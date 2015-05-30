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

#ifndef _memory_h
#define _memory_h

#include <stdint.h>
#include "bootloader_common.h"
#include "bootloader_config.h"

//! @addtogroup memif
//! @{

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

//! @brief Memory interface status codes.
enum _memory_interface_status
{
    kStatusMemoryRangeInvalid = MAKE_STATUS(kStatusGroup_MemoryInterface, 0),
    kStatusMemoryReadFailed = MAKE_STATUS(kStatusGroup_MemoryInterface, 1),
    kStatusMemoryWriteFailed = MAKE_STATUS(kStatusGroup_MemoryInterface, 2)
};

//! @brief Interface to memory operations.
//!
//! This is the main abstract interface to all memory operations.
typedef struct _memory_interface
{
    status_t (*init)(void);
#if BL_FEATURE_READ_MEMORY_CMD
    status_t (*read)(uint32_t address, uint32_t length, uint8_t * buffer);
#endif
    status_t (*write)(uint32_t address, uint32_t length, const uint8_t * buffer);
#if BL_FEATURE_FILL_MEMORY_CMD
    status_t (*fill)(uint32_t address, uint32_t length, uint32_t pattern);
#endif
    status_t (*flush)(void);
    status_t (*erase)(uint32_t address, uint32_t length);
} memory_interface_t;

//! @brief Interface to memory operations for one region of memory.
typedef struct _memory_region_interface
{
    status_t (*init)(void);
#if BL_FEATURE_READ_MEMORY_CMD
    status_t (*read)(uint32_t address, uint32_t length, uint8_t * buffer);
#endif
    status_t (*write)(uint32_t address, uint32_t length, const uint8_t * buffer);
#if BL_FEATURE_FILL_MEMORY_CMD
    status_t (*fill)(uint32_t address, uint32_t length, uint32_t pattern);
#endif
    status_t (*flush)(void);
    status_t (*erase)(uint32_t address, uint32_t length);
} memory_region_interface_t;

//! @brief Structure of a memory map entry.
typedef struct _memory_map_entry
{
    uint32_t startAddress;
    uint32_t endAddress;
    const memory_region_interface_t * memoryInterface;
} memory_map_entry_t;

//! @brief Memory Map index constants
enum _memorymap_constants
{
    kIndexFlashArray = 0,
    kIndexSRAM = 1,
    kIndexQspiMemory = 2,
    kSRAMSeparatrix = (uint32_t)0x20000000  //!< This value is the start address of SRAM_U
};

////////////////////////////////////////////////////////////////////////////////
// Externs
////////////////////////////////////////////////////////////////////////////////

//! @brief Memory map for the system.
extern memory_map_entry_t g_memoryMap[];

//! @name Memory interfaces
//@{

//! @brief Abstract memory interface.
//!
//! This interface utilizes the memory map to perform different memory operations
//! depending on the region of memory being accessed.
extern const memory_interface_t g_memoryInterface;

//! @brief Flash memory interface.
extern const memory_region_interface_t g_flashMemoryInterface;

//! @brief Memory interface for memory with Normal type.
//!
//! Use of multiword loads and stores is allowed with this memory type.
extern const memory_region_interface_t g_normalMemoryInterface;

//! @brief Memory interface for memory with Device or Strongly-ordered type.
//!
//! This memory type does not support multiword loads and stores.
extern const memory_region_interface_t g_deviceMemoryInterface;

//! @brief SPI Flash memory interface
extern const memory_region_interface_t g_spiFlashMemoryInterface;

//! @brief I2C EEPROM memory interface
extern const memory_region_interface_t g_i2cEepromMemoryInterface;

//! @brief SRAM memory interface
extern const memory_region_interface_t g_sramMemoryInterface;

//@}

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

//! @name Generic memory interface implementation
//@{

//! @brief Initialize memory interface.
status_t mem_init(void);

//! @brief Read memory.
status_t mem_read(uint32_t address, uint32_t length, uint8_t * buffer);

//! @brief Write memory.
status_t mem_write(uint32_t address, uint32_t length, const uint8_t * buffer);

//! @brief Fill memory with a word pattern.
status_t mem_fill(uint32_t address, uint32_t length, uint32_t pattern);

//! @brief Erase memory:
status_t mem_erase(uint32_t address, uint32_t length);

//! @brief Flush meory:
status_t mem_flush(void);

//@}

//! @name Memory utilities
//@{

//! @brief Determine if all or part of block is in a reserved region.
bool mem_is_block_reserved(uint32_t address, uint32_t length);

//@}

//! @name Flash erase operations
//@{

//! @brief Erase Flash memory.
status_t flash_mem_erase(uint32_t address, uint32_t length);

//! @brief Erase all Flash memory.
//!
//! If building for flash resident bootloader, we have to decompose the the flash erase all
//! operation into two region erases. This allows the user to still do an erase all, but not
//! wipe out the bootloader itself. Note that this _won't_ happen if the user has disabled
//! the validateRegions property.
status_t flash_mem_erase_all(void);

//! @brief Erase all Flash memory (unsecure).
status_t flash_mem_erase_all_unsecure(void);

//@}

//! @name QSPI erase operation
//@{

//! @brief Erase all QSPI memory
status_t qspi_mem_erase_all(void);
//@}


#if defined(__cplusplus)
}
#endif // __cplusplus

#if defined(BOOTLOADER_HOST)

////////////////////////////////////////////////////////////////////////////////
// Simulator host prototypes
////////////////////////////////////////////////////////////////////////////////

#if __cplusplus
extern "C" {
#endif

//! @brief Erase all flash.
void host_flash_erase_all(void);

//! @brief Erase all flash (unsecure).
void host_flash_erase_all_unsecure(void);

//! @brief Erase a region of flash.
void host_flash_erase_region(uint32_t address, uint32_t count);

#if __cplusplus
}
#endif

#endif // BOOTLOADER_HOST

//! @}

#endif // _memory_h
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
