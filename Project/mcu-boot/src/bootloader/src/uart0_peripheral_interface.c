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
#include "uart/uart0.h"
#include "utilities/fsl_assert.h"

//! @addtogroup uart0_peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static bool uart0_poll_for_activity(const peripheral_descriptor_t * self);
static status_t uart0_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function);
static void uart0_full_shutdown(const peripheral_descriptor_t * self);

static status_t uart0_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const peripheral_control_interface_t g_uart0ControlInterface = {
    .pollForActivity = uart0_poll_for_activity,
    .init = uart0_full_init,
    .shutdown = uart0_full_shutdown,
    .pump = 0
};

const peripheral_byte_inteface_t g_uart0ByteInterface = {
    .init = NULL,
    .write = uart0_write
};

static serial_byte_receive_func_t s_uart0_byte_receive_callback;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

bool uart0_poll_for_activity(const peripheral_descriptor_t * self)
{
    unsigned int baud;
    status_t autoBaudCompleted = autobaud_get_rate(self->instance, &baud);

    if (autoBaudCompleted == kStatus_Success)
    {
        if (uart0_init(get_uart_clock(self->instance), baud, s_uart0_byte_receive_callback) == kStatus_Success)
        {
            // Configure selected pin as uart peripheral interface
            self->pinmuxConfig(self->instance, kPinmuxType_Peripheral);

            // This was the byte pattern identified in autobaud detection, inform the command layer
            s_uart0_byte_receive_callback(kFramingPacketStartByte);
            s_uart0_byte_receive_callback(kFramingPacketType_Ping);

            return true;
        }
        else
        {
            // If the uart init did not complete that means that the detected baud rate was invalid
            // restart autobaud and resume detection
            autobaud_init(self->instance);

            return false;
        }
    }

    return false;
}

status_t uart0_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function)
{
    s_uart0_byte_receive_callback = function;

    // Since we are using autobaud once the detection is completed
    // it will call the UART initialization and remux the pins when it completes
    self->pinmuxConfig(self->instance, kPinmuxType_GPIO);

    // Init autobaud detector.
    autobaud_init(self->instance);

    return kStatus_Success;
}

void uart0_full_shutdown(const peripheral_descriptor_t * self)
{
    uart0_shutdown();
    //! Note: if not deinit autobaud(IRQ method), user app may encounters hardfault
    //! if it doesn't provide related pin interrupt service routine.
#if BL_UART_AUTOBAUD_IRQ
    // De-init autobaud detector.
    autobaud_deinit(self->instance);
#endif
    // Restore selected pin to default state to reduce IDD.
    self->pinmuxConfig(self->instance, kPinmuxType_Default);
}

status_t uart0_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount)
{
    uint32_t remaining = byteCount;
    while (remaining--)
    {
        uart0_putchar(*buffer++);
    }

    return kStatus_Success;
}

//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
