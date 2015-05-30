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
#ifndef __BOOTLOADER_COMMON_H__
#define __BOOTLOADER_COMMON_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "fsl_platform_types.h"
#include "fsl_platform_status.h"
#include "fsl_platform_common.h"

#if defined(BOOTLOADER_HOST)
#include "blfwk/bootloader_config.h"
#else
#include "bootloader_config.h"
#include "target_config.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief the following macros are to be used when trying to save code size for specific peripheral configurations
//! that will only be using one peripheral instance. most of the peripheral driver code can use multiple instances but by just using one
//! we can save space
#define USE_ONLY_UART(instance) (defined(BL_UART_SIZE_OPTIMIZE) && (BL_UART_USED_INSTANCE == instance))
#define USE_ONLY_SPI(instance)  (defined(BL_SPI_SIZE_OPTIMIZE)  && (BL_SPI_USED_INSTANCE  == instance))
#define USE_ONLY_I2C(instance)  (defined(BL_I2C_SIZE_OPTIMIZE)  && (BL_I2C_USED_INSTANCE  == instance))

//! @brief Callback function invoked for a pin change interrupt.
//!
//! @ingroup bl_hw
typedef void (*pin_irq_callback_t)(void);

//! @brief Bootloader status group numbers.
enum _bl_status_groups
{
    kStatusGroup_Bootloader = kStatusGroup_ApplicationRangeStart,        //!< Bootloader status group number (100).
    kStatusGroup_SBLoader,          //!< SB loader status group number (101).
    kStatusGroup_MemoryInterface,   //!< Memory interface status group number (102).
    kStatusGroup_PropertyStore,     //!< Property store status group number (103).
    kStatusGroup_AppCrcCheck,       //!< Application crc check status group number (104).
};

//! @brief Structure of version property.
typedef union StandardVersion
{
    struct {
        uint8_t bugfix;     //!< bugfix version [7:0]
        uint8_t minor;      //!< minor version [15:8]
        uint8_t major;      //!< major version [23:16]
        char name;          //!< name [31:24]
    };
    uint32_t version;   //!< combined version numbers

#if defined(__cplusplus)
    StandardVersion() : version(0) {}
    StandardVersion(uint32_t version) : version(version) {}
#endif
} standard_version_t;

#define MAKE_VERSION(bugfix, minor, major, name) (((name) << 24) | ((major) << 16) | ((minor) << 8 ) | (bugfix))

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

//! @addtogroup bl_hw
//! @{

//! @brief Initialize the hardware such as pinmux.
void init_hardware(void);

//! @brief DeInitialize the hardware such as disabling port clock gate
void deinit_hardware(void);

//! @brief Update available peripherals based on specific chips
void update_available_peripherals(void);

//! @brief Returns the logic level of the board specific GPIO pin used for autobaud.
unsigned int read_autobaud_pin(unsigned int instance);

//! @brief Configure hardware clocks.
void configure_clocks(void);

//! @brief Returns the available lirc clock frequency in Hertz.
uint32_t get_available_lirc_clock(void);

//! @brief Returns the current bus clock frequency in Hertz.
uint32_t get_bus_clock(void);

//! @brief Returns the current core clock frequency in Hertz.
uint32_t get_system_core_clock(void);

//! @brief Configure usb clock
bool usb_clock_init(void);

//! @brief Returns the value in MHz of the UART clock based on the instance.
uint32_t get_uart_clock(unsigned int instance);

//! @brief Returns true if reset BOOTROM mode is selected.
bool is_boot_pin_asserted(void);

#if BL_ENABLE_CRC_CHECK
//! @brief Toggle a pin to the inverse of default logic state to indicate crc check failure.
void assert_pin_to_indicate_crc_check_failure(void);
#endif

//! @brief Enables the autobaud pin IRQ for the specific instance passed.
void enable_autobaud_pin_irq(pin_irq_callback_t func);

//! @brief Disables the autobaud pin IRQ for the instance passed.
void disable_autobaud_pin_irq(void);

//! @brief Declaration for the reset handler, which is defined in assembler.
void Reset_Handler(void);

//! @brief Initialize watchdog
void bootloader_watchdog_init(void);

//! @brief Service watchdog
void bootloader_watchdog_service(void);

//! @brief De-initialize watchdog
void bootloader_watchdog_deinit(void);

//! @brief Determine if QSPI module to be configured.
bool qspi_need_configure(void);

//! @brief Initialize QSPI and OTFAD module.
//! @param configBase Address of config block or NULL to read config block from qspi flash
status_t qspi_otfad_init(void * configBase);

//! @brief Determine if QSPI memory is present or not.
bool is_qspi_present(void);

//! @brief Determine if OTFAD module is present or not.
bool is_otfad_present(void);

//! @brief Determine if LTC module is present or not.
bool is_ltc_present(void);

//! @brief Return status for intializing qspi and otfad modules
status_t get_qspi_otfad_init_status(void);

//!@bief Update status for intializing qspi and otfad modules
void update_qspi_otfad_init_status(status_t initStatus);

//! @}

#endif // __BOOTLOADER_COMMON_H__
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

