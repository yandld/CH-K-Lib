/*
 * Copyright (c) 2013-2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "bootloader/context.h"
#include "bootloader_common.h"
#include "bootloader_config.h"
#include "bootloader/bl_peripheral_interface.h"
#include "autobaud/autobaud.h"
#include "packet/serial_packet.h"
#include "device/fsl_device_registers.h"
#include "lpuart/hal/fsl_lpuart_hal.h"
#include "utilities/fsl_assert.h"

//! @addtogroup lpuart_peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static bool lpuart_poll_for_activity(const peripheral_descriptor_t * self);
static status_t lpuart_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function);
static void lpuart_full_shutdown(const peripheral_descriptor_t * self);

static status_t lpuart_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount);

extern void lpuart_set_system_IRQ_gate(uint32_t instance, PeripheralSystemIRQSetting set);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const peripheral_control_interface_t g_lpuartControlInterface = {
    .pollForActivity = lpuart_poll_for_activity,
    .init = lpuart_full_init,
    .shutdown = lpuart_full_shutdown,
    .pump = 0
};

const peripheral_byte_inteface_t g_lpuartByteInterface = {
    .init = NULL,
    .write = lpuart_write
};

static serial_byte_receive_func_t s_lpuart_byte_receive_callback;



static const uint32_t g_lpuartBaseAddr[] = LPUART_BASE_ADDRS;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

static void lpuart_set_clock_gate(uint32_t instance, PeripheralClockSetting set)
{
    uint32_t mask;
    uint32_t * address;

    switch(instance)
    {
#if defined(PCC_BASE_ADDRS)
        case 0:

            break;
// If support multiple instance
#if (HW_LPUART_INSTANCE_COUNT > 1)
        case 1:

            break;
#endif // #if (HW_LPUART_INSTANCE_COUNT > 1)
#if (HW_LPUART_INSTANCE_COUNT > 2)
        case 2:
            mask = BM_PCC_CLKCFGn_CGC;
            address = (uint32_t*)HW_PCC_CLKCFGn_ADDR(PCC0_BASE, HW_PCC_CLKCFG_ADDR_OFFSET_LPUART2);
            break;
#endif // #if (HW_LPUART_INSTANCE_COUNT > 2)

#else

        case 0:
#if defined(BM_SIM_SCGC5_LPUART0)
            mask = BM_SIM_SCGC5_LPUART0;
            address = (uint32_t*)HW_SIM_SCGC5_ADDR(SIM_BASE);
#elif defined(BM_SIM_SCGC6_LPUART0)
            mask = BM_SIM_SCGC6_LPUART0;
            address = (uint32_t*)HW_SIM_SCGC6_ADDR(SIM_BASE);
#elif defined(BM_SIM_SCGC2_LPUART0)
            mask = BM_SIM_SCGC2_LPUART0;
            address = (uint32_t*)HW_SIM_SCGC2_ADDR(SIM_BASE);
#endif
            break;
// If support multiple instance
#if (HW_LPUART_INSTANCE_COUNT > 1)
        case 1:
#if defined(BM_SIM_SCGC5_LPUART1)
            mask = BM_SIM_SCGC5_LPUART1;
            address = (uint32_t*)HW_SIM_SCGC5_ADDR(SIM_BASE);
#elif defined ( BM_SIM_SCGC2_LPUART1)
            mask = BM_SIM_SCGC2_LPUART1;
            address = (uint32_t*)HW_SIM_SCGC2_ADDR(SIM_BASE);
#endif
            break;
#endif // #if (HW_LPUART_INSTANCE_COUNT > 1)
#if (HW_LPUART_INSTANCE_COUNT > 2)
        case 2:
#if defined(BM_SIM_SCGC2_LPUART2)
            mask = BM_SIM_SCGC2_LPUART2;
            address = (uint32_t*)HW_SIM_SCGC2_ADDR(SIM_BASE);
#endif
            break;
#endif // #if (HW_LPUART_INSTANCE_COUNT > 2)

#endif // #if defined(PCC_BASE_ADDRS)
    }

    if (set == kPeripheralSetClock)
    {
        *address = *address | mask;
    }
    else
    {
        *address = *address & ~mask;
    }
}

/*!
 * This function will be called from the main peripheral detection loop to drive
 * the autobaud detect for this UART instance. If it is completed the UART is
 * fully initialized and ready to use.
 */
bool lpuart_poll_for_activity(const peripheral_descriptor_t * self)
{
    uint32_t instance;
#if USE_ONLY_UART(0)
    instance = 0;
#elif USE_ONLY_UART(1)
    instance = 1;
#else
    instance = self->instance;
#endif // USE_ONLY_UART(0)

    uint32_t baseAddr = g_lpuartBaseAddr[instance];

    // Check for autobaud completion.
    unsigned int baud;
    status_t autoBaudCompleted = autobaud_get_rate(instance, &baud);

    // If autobaud is still running then exit immediately.
    if (autoBaudCompleted != kStatus_Success)
    {
        return false;
    }

    // Ungate the LPUART clock.
    lpuart_set_clock_gate(instance, kPeripheralSetClock);

    // Must disable transmitter and receiver before changing the baud rate.
    lpuart_hal_disable_transmitter(baseAddr);
    lpuart_hal_disable_receiver(baseAddr);

    if(lpuart_hal_set_baud_rate(baseAddr, get_uart_clock(instance), baud) == kStatus_Success)
    {
        // Configure selected pin as uart peripheral interface
        self->pinmuxConfig(instance, kPinmuxType_Peripheral);

        // Enable LPUART interrupt
        lpuart_set_system_IRQ_gate(instance, kPeripheralEnableIRQ);

        lpuart_hal_enable_receive_interrupt(baseAddr);

        // Reenable transmitter and receiver.
        lpuart_hal_enable_transmitter(baseAddr);
        lpuart_hal_enable_receiver(baseAddr);

        // This was the byte pattern identified in autobaud detection, inform the command layer
        s_lpuart_byte_receive_callback(kFramingPacketStartByte);
        s_lpuart_byte_receive_callback(kFramingPacketType_Ping);

        // Return true to indicate autobaud is complete and the UART is active.
        return true;
    }
    else
    {
        // Ungate the LPUART clock.
        lpuart_set_clock_gate(instance, kPeripheralClearClock);

        // Reinitialize autobaud because the baud rate was invalid
        autobaud_init(instance);

        return false;
    }
}

//! Note that we don't ungate the LPUART clock gate here. That is done only after the
//! autobaud process has completed successfully.
status_t lpuart_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function)
{
    s_lpuart_byte_receive_callback = function;

    // Since we are using autobaud once the detection is completed
    // it will call the UART initialization and remux the pins when it completes
    self->pinmuxConfig(self->instance, kPinmuxType_GPIO);

    // Init autobaud detector.
    autobaud_init(self->instance);

    return kStatus_Success;
}

void lpuart_full_shutdown(const peripheral_descriptor_t * self)
{
    uint32_t instance;
#if USE_ONLY_UART(0)
    instance = 0;
#elif USE_ONLY_UART(1)
    instance = 1;
#else
    instance = self->instance;
#endif // USE_ONLY_UART(0)
    uint32_t baseAddr = g_lpuartBaseAddr[instance];

    lpuart_set_clock_gate(instance, kPeripheralSetClock);

    // Disable LPUART interrupt
    lpuart_set_system_IRQ_gate(instance, kPeripheralDisableIRQ);

    // Reset LPUART registers
    lpuart_hal_reset(baseAddr);

    // Gate the LPUART clock.
    lpuart_set_clock_gate(instance, kPeripheralClearClock);

    //! Note: if not deinit autobaud(IRQ method), user app may encounters hardfault
    //! if it doesn't provide related pin interrupt service routine.
#if BL_UART_AUTOBAUD_IRQ
    // De-init autobaud detector.
    autobaud_deinit(instance);
#endif

    // Restore selected pin to default state to reduce IDD.
    self->pinmuxConfig(instance, kPinmuxType_Default);
}

status_t lpuart_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount)
{
    uint32_t instance;
#if USE_ONLY_UART(0)
    instance = 0;
#elif USE_ONLY_UART(1)
    instance = 1;
#else
    instance = self->instance;
#endif // USE_ONLY_UART(0)

    uint32_t baseAddr = g_lpuartBaseAddr[instance];
    uint32_t remaining = byteCount;

    while (remaining)
    {
        if (lpuart_hal_is_transmit_data_register_empty(baseAddr))
        {
            lpuart_hal_putchar(baseAddr, *buffer++);
            remaining--;
        }
    }

    return kStatus_Success;
}

#if !defined(BL_UART_SIZE_OPTIMIZE) || USE_ONLY_UART(0)
/********************************************************************/
/*
 * LPUART0 IRQ Handler
 *
 */
void LPUART0_IRQHandler(void)
{
    if (lpuart_hal_is_receive_data_register_full(LPUART0_BASE))
    {
        s_lpuart_byte_receive_callback(lpuart_hal_getchar(LPUART0_BASE));
    }
}
#endif // !defined(BL_UART_SIZE_OPTIMIZE) || USE_ONLY_UART(0)

#if (HW_LPUART_INSTANCE_COUNT > 1)
void LPUART1_IRQHandler(void)
{
    if (lpuart_hal_is_receive_data_register_full(LPUART1_BASE))
    {
        s_lpuart_byte_receive_callback(lpuart_hal_getchar(LPUART1_BASE));
    }
}
#endif // #if (HW_LPUART_INSTANCE_COUNT > 1)

#if (HW_LPUART_INSTANCE_COUNT > 2)
void LPUART2_IRQHandler(void)
{
    if (lpuart_hal_is_receive_data_register_full(LPUART2_BASE))
    {
        s_lpuart_byte_receive_callback(lpuart_hal_getchar(LPUART2_BASE));
    }
}
#endif // #if (HW_LPUART_INSTANCE_COUNT > 2)



//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
