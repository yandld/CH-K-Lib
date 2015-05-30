/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
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
#include "autobaud/autobaud.h"
#include "packet/serial_packet.h"
#include "device/fsl_device_registers.h"
#include "uart/scuart.h"
#include "utilities/fsl_assert.h"

//! @addtogroup scuart_peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static UART_Type * get_uart_instance(uint32_t instance);
static bool scuart_poll_for_activity(const peripheral_descriptor_t * self);
static status_t scuart_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function);
static void scuart_full_shutdown(const peripheral_descriptor_t * self);

static status_t scuart_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const peripheral_control_interface_t g_scuartControlInterface = {
    .pollForActivity = scuart_poll_for_activity,
    .init = scuart_full_init,
    .shutdown = scuart_full_shutdown,
};

const peripheral_byte_inteface_t g_scuartByteInterface = {
    .init = NULL,
    .write = scuart_write
};

static serial_byte_receive_func_t s_scuart_byte_receive_callback;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
static UART_Type * get_uart_instance(uint32_t instance)
{
    UART_Type * s_UARTs[] = UART_BASE_PTRS;
    return s_UARTs[instance]; 
}

bool scuart_poll_for_activity(const peripheral_descriptor_t * self)
{
    unsigned int baud;
    status_t autoBaudCompleted = autobaud_get_rate(&baud);

    if (autoBaudCompleted == kStatus_Success)
    {
        if (scuart_init(get_uart_instance(self->instance), get_uart_clock(self->instance), baud, s_scuart_byte_receive_callback) == kStatus_Success)
        {
            // Configure selected pin as uart peripheral interface
            self->pinmuxConfig(self->instance, kPinmuxType_Peripheral);

            // This was the byte pattern identified in autobaud detection, inform the command layer
            s_scuart_byte_receive_callback(kFramingPacketStartByte);
            s_scuart_byte_receive_callback(kFramingPacketType_Ping);

            return true;
        }
        else
        {
            // If the uart init did not complete that means that the detected baud rate was invalid
            // restart autobaud and resume detection
            autobaud_init();

            return false;
        }
    }

    return false;
}

status_t scuart_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function)
{
    s_scuart_byte_receive_callback = function;

    // Since we are using autobaud once the detection is completed
    // it will call the UART initialization and remux the pins when it completes
    self->pinmuxConfig(self->instance, kPinmuxType_GPIO);

    // Init autobaud detector.
    autobaud_init();

    return kStatus_Success;
}

void scuart_full_shutdown(const peripheral_descriptor_t * self)
{
    scuart_shutdown(get_uart_instance(self->instance));
    //! Note: if not deinit autobaud(IRQ method), user app may encounters hardfault
    //! if it doesn't provide related pin interrupt service routine.
#if BL_UART_AUTOBAUD_IRQ
    // De-init autobaud detector.
    autobaud_deinit();
#endif
    // Restore selected pin to default state to reduce IDD.
    self->pinmuxConfig(self->instance, kPinmuxType_Default);
}

status_t scuart_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount)
{
    uint32_t remaining = byteCount;
    while (remaining--)
    {
        scuart_putchar(get_uart_instance(self->instance), *buffer++);
    }

    return kStatus_Success;
}

//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
