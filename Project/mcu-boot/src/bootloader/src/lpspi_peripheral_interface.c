/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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

#include "utilities/fsl_assert.h"
#include "bootloader/context.h"
#include "bootloader_common.h"
#include "bootloader/bl_peripheral_interface.h"
#include "packet/command_packet.h"
#include "lpspi/fsl_lpspi_slave_driver.h"
#include "device/fsl_device_registers.h"

//! @addtogroup lpspi_peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Synchronization state between LPSPI ISR and read/write functions.
typedef struct _lpspi_transfer_info {
    const uint8_t * writeData;            //!< The applications data to write
    volatile uint32_t bytesToTransfer;    //!< The total number of bytes to be transmitted
} lpspi_transfer_info_t;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static bool lpspi_poll_for_activity(const peripheral_descriptor_t * self);
static status_t lpspi_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function);
static void lpspi_full_shutdown(const peripheral_descriptor_t * self);

static void lpspi_data_source(uint8_t * source_byte, uint16_t instance);
static void lpspi_data_sink(uint8_t sink_byte, uint16_t instance);

static status_t lpspi_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const peripheral_control_interface_t g_lpspiControlInterface = {
    .pollForActivity = lpspi_poll_for_activity,
    .init = lpspi_full_init,
    .shutdown = lpspi_full_shutdown,
    .pump = 0
};

const peripheral_byte_inteface_t g_lpspiByteInterface = {
    .init = NULL,
    .write = lpspi_write
};

static lpspi_transfer_info_t s_lpspiInfo = {
    .writeData = 0,
    .bytesToTransfer = 0
};

//! @brief Flag for detecting device activity
static bool s_lpspiActivity[HW_LPSPI_INSTANCE_COUNT] = {false};

static serial_byte_receive_func_t s_lpspi_app_data_sink_callback;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

static void lpspi_set_clock_gate(uint32_t instance, PeripheralClockSetting set)
{
    uint32_t mask;
    uint32_t * address;

    switch(instance)
    {
#if defined(PCC_BASE_ADDRS)
        case 0:

            break;
// If support multiple instance
#if (HW_LPSPI_INSTANCE_COUNT > 1)
        case 1:

            break;
#endif // #if (HW_LPSPI_INSTANCE_COUNT > 1)
#if (HW_LPSPI_INSTANCE_COUNT > 2)
        case 2:
            mask = BM_PCC_CLKCFGn_CGC;
            address = (uint32_t*)HW_PCC_CLKCFGn_ADDR(PCC0_BASE, HW_PCC_CLKCFG_ADDR_OFFSET_LPSPI2);
            break;
#endif // #if (HW_LPSPI_INSTANCE_COUNT > 2)
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

bool lpspi_poll_for_activity(const peripheral_descriptor_t * self)
{
    return s_lpspiActivity[self->instance];
}

void lpspi_data_source(uint8_t * source_byte, uint16_t instance)
{
    assert(source_byte);

    if (s_lpspiInfo.bytesToTransfer)
    {
        *source_byte = *s_lpspiInfo.writeData++;
        s_lpspiInfo.bytesToTransfer--;
    }
    else
    {
        *source_byte = 0;
    }
}

void lpspi_data_sink(uint8_t sink_byte, uint16_t instance)
{
    s_lpspiActivity[instance] = true;

    s_lpspi_app_data_sink_callback(sink_byte);
}

status_t lpspi_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function)
{
    s_lpspi_app_data_sink_callback = function;

    lpspi_slave_config_t config = {
            .callbacks = {
                    .dataSource = lpspi_data_source,
                    .dataSink = lpspi_data_sink
                },
            .phase = kLpspiClockPhase_SecondEdge,
            .polarity = kLpspiClockPolarity_ActiveLow,
            .direction = kLpspiMsbFirst
    };
    // Configure selected pin as lpspi peripheral interface
    self->pinmuxConfig(self->instance, kPinmuxType_Peripheral);
    // Ungate the LPSPI clock.
    lpspi_set_clock_gate(self->instance, kPeripheralSetClock);
    lpspi_slave_init(self->instance, &config);

    return kStatus_Success;
}

void lpspi_full_shutdown(const peripheral_descriptor_t * self)
{
    // Ungate the LPSPI clock.
    lpspi_set_clock_gate(self->instance, kPeripheralSetClock);
    lpspi_slave_deinit(self->instance);
    // Gate the LPSPI clock.
    lpspi_set_clock_gate(self->instance, kPeripheralClearClock);
    // Restore selected pin to default state to reduce IDD.
    self->pinmuxConfig(self->instance, kPinmuxType_Default);
}

status_t lpspi_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount)
{
    s_lpspiInfo.writeData = buffer;
    s_lpspiInfo.bytesToTransfer = byteCount;

    while (s_lpspiInfo.bytesToTransfer);

    return kStatus_Success;
}

//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
