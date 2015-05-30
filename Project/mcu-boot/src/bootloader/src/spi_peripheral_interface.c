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

#include "utilities/fsl_assert.h"
#include "bootloader/context.h"
#include "bootloader_common.h"
#include "packet/command_packet.h"
#include "spi/fsl_spi_slave_driver.h"
#include "device/fsl_device_registers.h"

//! @addtogroup spi_peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Synchronization state between SPI ISR and read/write functions.
typedef struct _spi_transfer_info {
    const uint8_t * writeData;            //!< The applications data to write
    volatile uint32_t bytesToTransfer;    //!< The total number of bytes to be transmitted
} spi_transfer_info_t;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static bool spi_poll_for_activity(const peripheral_descriptor_t * self);
static status_t spi_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function);
static void spi_full_shutdown(const peripheral_descriptor_t * self);

static void spi_data_source(uint8_t * source_byte, uint16_t instance);
static void spi_data_sink(uint8_t sink_byte, uint16_t instance);

static status_t spi_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const peripheral_control_interface_t g_spiControlInterface = {
    .pollForActivity = spi_poll_for_activity,
    .init = spi_full_init,
    .shutdown = spi_full_shutdown,
    .pump = 0
};

const peripheral_byte_inteface_t g_spiByteInterface = {
    .init = NULL,
    .write = spi_write
};

static spi_transfer_info_t s_spiInfo = {
    .writeData = 0,
    .bytesToTransfer = 0
};

//! @brief Flag for detecting device activity
static bool s_spiActivity[HW_SPI_INSTANCE_COUNT] = {false};

static serial_byte_receive_func_t s_spi_app_data_sink_callback;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

bool spi_poll_for_activity(const peripheral_descriptor_t * self)
{
    return s_spiActivity[self->instance];
}

void spi_data_source(uint8_t * source_byte, uint16_t instance)
{
    assert(source_byte);

    if (s_spiInfo.bytesToTransfer)
    {
        *source_byte = *s_spiInfo.writeData++;
        s_spiInfo.bytesToTransfer--;
    }
    else
    {
        *source_byte = 0;
    }
}

void spi_data_sink(uint8_t sink_byte, uint16_t instance)
{
    s_spiActivity[instance] = true;

    s_spi_app_data_sink_callback(sink_byte);
}

status_t spi_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function)
{
    s_spi_app_data_sink_callback = function;

    spi_slave_config_t config = {
            .callbacks = {
                    .dataSource = spi_data_source,
                    .dataSink = spi_data_sink
                },
            .phase = kSpiClockPhase_SecondEdge,
            .polarity = kSpiClockPolarity_ActiveLow,
            .direction = kSpiMsbFirst
    };
    // Configure selected pin as spi peripheral interface
    self->pinmuxConfig(self->instance, kPinmuxType_Peripheral);
    spi_slave_init(self->instance, &config);

    return kStatus_Success;
}

void spi_full_shutdown(const peripheral_descriptor_t * self)
{
    spi_slave_shutdown(self->instance);
    // Restore selected pin to default state to reduce IDD.
    self->pinmuxConfig(self->instance, kPinmuxType_Default);
}

status_t spi_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount)
{
    s_spiInfo.writeData = buffer;
    s_spiInfo.bytesToTransfer = byteCount;

    while (s_spiInfo.bytesToTransfer);

    return kStatus_Success;
}

//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
