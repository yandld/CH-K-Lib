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

#include <assert.h>
#include "bootloader/context.h"
#include "bootloader_common.h"
#include "bootloader/bl_peripheral_interface.h"
#include "packet/command_packet.h"
#include "lpi2c/slave/fsl_lpi2c_slave_driver.h"
#include "device/fsl_device_registers.h"

//! @addtogroup lpi2c_peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

// Allow the default to be overridden via the bootloader_config.h file.
#if !defined(BL_DEFAULT_I2C_SLAVE_ADDRESS)
//! @brief Default LPI2C slave address in 7-bit format.
#define BL_DEFAULT_I2C_SLAVE_ADDRESS (0x10)
#endif // BL_DEFAULT_I2C_SLAVE_ADDRESS

//! @brief Synchronization state between LPI2C ISR and read/write functions.
typedef struct _lpi2c_transfer_info {
    const uint8_t * writeData;         //!< The applications data to write
    volatile uint32_t bytesToTransfer; //!< The total number of bytes to be transmitted
} lpi2c_transfer_info_t;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

static bool lpi2c_poll_for_activity(const peripheral_descriptor_t * self);
static status_t lpi2c_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function);
static void lpi2c_full_shutdown(const peripheral_descriptor_t * self);

static void lpi2c_data_source(uint8_t * source_byte);
static void lpi2c_data_sink(uint8_t sink_byte, uint32_t instance);

static status_t lpi2c_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const peripheral_control_interface_t g_lpi2cControlInterface = {
    .pollForActivity = lpi2c_poll_for_activity,
    .init = lpi2c_full_init,
    .shutdown = lpi2c_full_shutdown,
    .pump = 0
};

const peripheral_byte_inteface_t g_lpi2cByteInterface = {
    .init = NULL,
    .write = lpi2c_write
};

//! @brief Global state for the LPI2C peripheral interface.
static lpi2c_transfer_info_t s_lpi2cInfo = {
    .writeData = 0,
    .bytesToTransfer = 0
};

static bool s_lpi2cActivity[HW_LPI2C_INSTANCE_COUNT] = {false};

static serial_byte_receive_func_t s_lpi2c_app_data_sink_callback;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

static void lpi2c_set_clock_gate(uint32_t instance, PeripheralClockSetting set)
{
    uint32_t mask;
    uint32_t * address;

    switch(instance)
    {
#if defined(PCC_BASE_ADDRS)
        case 0:

            break;
// If support multiple instance
#if (HW_LPI2C_INSTANCE_COUNT > 1)
        case 1:

            break;
#endif // #if (HW_LPI2C_INSTANCE_COUNT > 1)
#if (HW_LPI2C_INSTANCE_COUNT > 2)
        case 2:
            mask = BM_PCC_CLKCFGn_CGC;
            address = (uint32_t*)HW_PCC_CLKCFGn_ADDR(PCC0_BASE, HW_PCC_CLKCFG_ADDR_OFFSET_LPI2C2);
            break;
#endif // #if (HW_LPI2C_INSTANCE_COUNT > 2)
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

bool lpi2c_poll_for_activity(const peripheral_descriptor_t * self)
{
    return s_lpi2cActivity[self->instance];
}

void lpi2c_data_source(uint8_t * source_byte)
{
    assert(source_byte);

    if (s_lpi2cInfo.bytesToTransfer)
    {
        *source_byte = *s_lpi2cInfo.writeData++;
        s_lpi2cInfo.bytesToTransfer--;
    }
    else
    {
        *source_byte = 0;
    }
}

void lpi2c_data_sink(uint8_t sink_byte, uint32_t instance)
{
    s_lpi2cActivity[instance] = true;

    s_lpi2c_app_data_sink_callback(sink_byte);
}

status_t lpi2c_full_init(const peripheral_descriptor_t * self, serial_byte_receive_func_t function)
{
    s_lpi2c_app_data_sink_callback = function;

    // Read the address from the configuration field. If it is not set, i.e. 0xff,
    // use the default address.
    uint8_t slaveAddress = g_bootloaderContext.propertyInterface->store->configurationData.i2cSlaveAddress;
    if (slaveAddress == 0xff)
    {
        slaveAddress = BL_DEFAULT_I2C_SLAVE_ADDRESS;
    }

    // Init the driver.
    lpi2c_slave_info_t config = {
            .slaveAddress = slaveAddress,
            .data_source = lpi2c_data_source,
            .data_sink = lpi2c_data_sink,
        };
    // Configure selected pin as lpi2c peripheral interface
    self->pinmuxConfig(self->instance, kPinmuxType_Peripheral);
    // Ungate the LPI2C clock.
    lpi2c_set_clock_gate(self->instance, kPeripheralSetClock);
    lpi2c_slave_init(self->instance, &config);

    return kStatus_Success;
}

void lpi2c_full_shutdown(const peripheral_descriptor_t * self)
{
    // Ungate the LPI2C clock.
    lpi2c_set_clock_gate(self->instance, kPeripheralSetClock);
    lpi2c_slave_deinit(self->instance);
    // Gate the LPI2C clock.
    lpi2c_set_clock_gate(self->instance, kPeripheralClearClock);
    // Restore selected pin to default state to reduce IDD.
    self->pinmuxConfig(self->instance, kPinmuxType_Default);
}

status_t lpi2c_write(const peripheral_descriptor_t * self, const uint8_t * buffer, uint32_t byteCount)
{
    s_lpi2cInfo.writeData = buffer;
    s_lpi2cInfo.bytesToTransfer = byteCount;

    while (s_lpi2cInfo.bytesToTransfer);

    return kStatus_Success;
}

//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
