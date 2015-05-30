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

#include "bootloader_common.h"
#include "memory/memory.h"
#include "flash_memory.h"
#include "normal_memory.h"
#include "flash/flash.h"
#include "bootloader/context.h"
#include "device/fsl_device_registers.h"
#include "utilities/fsl_rtos_abstraction.h"
#include "utilities/fsl_assert.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

// If the bootloader is running from flash, then we need to make sure that all
// interrupts are disabled during the execution of a flash operation, so that
// no code is unexpectedly run from flash (which would cause a hard fault).
//
// If we're running from ROM or RAM, then we neither need to nor want to disable
// interrupts during flash operations.
#if !BL_TARGET_FLASH
    #define lock_release() (void)sizeof(0)
    #define lock_acquire() (void)sizeof(0)
#endif // BL_TARGET_FLASH

//! @brief Flash Memory constants.
enum _flash_memory_constants {
    kFlashMemory_ErasedValue = ~0
};

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

//! @brief Interface to flash memory operations.
const memory_region_interface_t g_flashMemoryInterface = {
    .init = &flash_mem_init,
#if BL_FEATURE_READ_MEMORY_CMD
    .read = &flash_mem_read,
#endif
    .write = &flash_mem_write,
#if BL_FEATURE_FILL_MEMORY_CMD
    .fill = &flash_mem_fill,
#endif // !BL_MIN_PROFILE
    .flush = NULL,
    .erase = flash_mem_erase,
};

#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
//! @brief It is used for indicating if an XA controlled region is unlocked to program state
bool isFlashRegionUnlocked = false;
#endif // FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////
#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
//! @brief check if a flash region is in an XA controlled region or contains an XA controlled region.
//         and try to open flash program state by calling verify_erase_all command if needed.
status_t flash_preprocess_execute_only_region(uint32_t address, uint32_t length, flash_execute_only_access_state_t *state);
#endif // #if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

// See flash_memory.h for documentation on this function.
status_t flash_mem_init(void)
{
    // Update address range of flash
    memory_map_entry_t * map = (memory_map_entry_t *)&g_bootloaderContext.memoryMap[kIndexFlashArray];
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_FlashBlockBaseAddr, &map->startAddress);
    uint32_t tmp;
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_TotalFlashSize, &tmp);
    map->endAddress = map->startAddress + tmp - 1;

    return kStatus_Success;
}


// See flash_memory.h for documentation on this function.
status_t flash_mem_read(uint32_t address, uint32_t length, uint8_t * restrict buffer)
{
#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
    flash_execute_only_access_state_t access_state;
    uint32_t alignedAddress = ALIGN_DOWN(address, g_bootloaderContext.flashState.PFlashAccessSegmentSize);
    uint32_t updatedLength = address - alignedAddress + length;
    uint32_t alignedLength = ALIGN_UP(updatedLength, g_bootloaderContext.flashState.PFlashAccessSegmentSize);
    status_t status = flash_is_execute_only(&g_bootloaderContext.flashState, alignedAddress, alignedLength,  &access_state);
    if (status != kStatus_Success)
    {
        return status;
    }

    if (access_state != kFlashAccess_UnLimited)
    {
        return kStatus_FlashRegionExecuteOnly;
    }
#endif // FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL

    return normal_mem_read(address, length, buffer);
}

// See flash_memory.h for documentation on this function.
status_t flash_mem_write(uint32_t address, uint32_t length, const uint8_t * buffer)
{
    // Note: the check for "length != 0" and "range not in reserved region" is done in mem_write().
    assert(length);
    assert(buffer);

    // Align length to whole words.
    uint32_t alignedLength = ALIGN_DOWN(length, sizeof(uint8_t)*FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE);
    uint32_t extraBytes = length - alignedLength;
    assert(extraBytes < sizeof(uint8_t)*FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE);

#if !BL_FLASH_VERIFY_DISABLE
    bool verifyWrites = g_bootloaderContext.propertyInterface->store->verifyWrites;
#endif // BL_FLASH_VERIFY_DISABLE

    // Pre-fill word buffer with flash erased value.
#if (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 4)
    uint32_t extraData;
    extraData = (uint32_t)kFlashMemory_ErasedValue;
#elif (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 8)
    uint64_t extraData;
    extraData = (uint64_t)kFlashMemory_ErasedValue;
#else
    #error "Untreated program unit size"
#endif
    if (extraBytes)
    {
        // Copy extra bytes to word buffer.
        memcpy((uint8_t *)&extraData, &buffer[alignedLength], extraBytes);
    }

    status_t status = kStatus_Success;

    lock_acquire();

#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
    // If a target flash location is in an execute-only protected segment, these program commands are not
    // allowed unless a Read 1s All Blocks command is executed and returns with a pass code
    flash_execute_only_access_state_t access_state;

    uint32_t actualLength = ALIGN_UP(length, (uint32_t)FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE);
    status = flash_preprocess_execute_only_region(address, actualLength, &access_state);
    if (status != kStatus_Success)
    {
        lock_release();
        return status;
    }

#if !BL_FLASH_VERIFY_DISABLE
    if (access_state != kFlashAccess_UnLimited)
    {
        verifyWrites = false;
    }
#endif // BL_FLASH_VERIFY_DISABLE

#endif  // FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL

    // Program whole words from the user's buffer.
    if (alignedLength)
    {
        status = flash_program(&g_bootloaderContext.flashState, address, (uint32_t *)buffer, alignedLength);
    }
    if ((status == kStatus_Success) && extraBytes)
    {
        // Program trailing word.
        status = flash_program(&g_bootloaderContext.flashState, address + alignedLength, (uint32_t *)&extraData, sizeof(extraData));
    }
    lock_release();
    if (status != kStatus_Success)
    {
        return status;
    }

#if !BL_FLASH_VERIFY_DISABLE

    if (verifyWrites)
    {
        uint32_t failedAddress;
        uint8_t  failedData;

        lock_acquire();
        if (alignedLength)
        {
            status = flash_verify_program(&g_bootloaderContext.flashState,
                                address, alignedLength, (uint8_t *)buffer,
                                (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin,
                                &failedAddress, &failedData);
        }
        if ((status == kStatus_Success) && extraBytes)
        {
            status = flash_verify_program(&g_bootloaderContext.flashState,
                                address + alignedLength,
                                sizeof(extraData), (uint8_t *)&extraData,
                                (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin,
                                &failedAddress, &failedData);
        }
        lock_release();
        if (status != kStatus_Success)
        {
            debug_printf("Error: flash verify failed at address: 0x%x\r\n", failedAddress);
            return status;
        }
    }
#endif // !BL_FLASH_VERIFY_DISABLE

    return kStatus_Success;
}

#if !BL_MIN_PROFILE
// See flash_memory.h for documentation on this function.
status_t flash_mem_fill(uint32_t address, uint32_t length, uint32_t pattern)
{
    // Note: the check for "length != 0" and "range not in reserved region" is done in mem_fill().
    assert(length);

    // Align end of buffer to the next word. The pattern will be written to the entire last word.
    uint32_t alignedLength;

#if !BL_FLASH_VERIFY_DISABLE
    bool verifyWrites = g_bootloaderContext.propertyInterface->store->verifyWrites;
#endif // BL_FLASH_VERIFY_DISABLE

#if (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 4)
    uint32_t patternBuffer = pattern;
    alignedLength = ALIGN_UP(length, sizeof(uint32_t));
#elif (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 8)
    uint64_t patternBuffer = pattern;
    patternBuffer <<= 32;
    patternBuffer += pattern;
    uint32_t extraBytes = 0;
    if ((length % FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE) > 4)
    {
        alignedLength = ALIGN_UP(length, sizeof(uint64_t));
    }
    else
    {
        alignedLength = ALIGN_DOWN(length, sizeof(uint64_t));
        extraBytes = length - alignedLength;
    }
#else
    #error "Untreated write-unit size"
#endif

    status_t status;

#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
    // If a target flash location is in an execute-only protected segment, these program commands are not
    // allowed unless a Read 1s All Blocks command is executed and returns with a pass code
    flash_execute_only_access_state_t access_state;

    uint32_t actualLength = ALIGN_UP(length, (uint32_t)FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE);
    status = flash_preprocess_execute_only_region(address, actualLength, &access_state);
    if (status != kStatus_Success)
    {
        return status;
    }

#if !BL_FLASH_VERIFY_DISABLE
    if (access_state != kFlashAccess_UnLimited)
    {
        verifyWrites = false;
    }
#endif // BL_FLASH_VERIFY_DISABLE

#endif  // FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL

    while (alignedLength)
    {
        lock_acquire();
        status = flash_program(&g_bootloaderContext.flashState, address, (uint32_t *)&patternBuffer, sizeof(patternBuffer));
        lock_release();
        if (status != kStatus_Success)
        {
            return status;
        }

#if !BL_FLASH_VERIFY_DISABLE
        // Verify flash program
        if (verifyWrites)
        {
            uint32_t failedAddress;
            uint8_t  failedData;

            lock_acquire();
            status = flash_verify_program(&g_bootloaderContext.flashState,
                              address, sizeof(patternBuffer), (uint8_t *)&patternBuffer,
                              (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin,
                              &failedAddress, &failedData);
            lock_release();
            if (status != kStatus_Success)
            {
                debug_printf("Error: flash verify failed at address: 0x%x\r\n", failedAddress);
                return status;
            }
        }
#endif // !BL_FLASH_VERIFY_DISABLE

        address += sizeof(patternBuffer);
        alignedLength -= sizeof(patternBuffer);
    }

#if (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 8)
    if (extraBytes)
    {
        uint64_t extraPatternBuffer = (uint32_t)kFlashMemory_ErasedValue;
        extraPatternBuffer <<= 32;
        extraPatternBuffer += pattern;

        lock_acquire();
        status = flash_program(&g_bootloaderContext.flashState, address, (uint32_t *)&extraPatternBuffer, sizeof(extraPatternBuffer));
        lock_release();
        if (status != kStatus_Success)
        {
            return status;
        }

#if !BL_FLASH_VERIFY_DISABLE
        // Verify flash program
        if (verifyWrites)
        {
            uint32_t failedAddress;
            uint8_t  failedData;

            lock_acquire();
            status = flash_verify_program(&g_bootloaderContext.flashState,
                              address, sizeof(extraPatternBuffer), (uint8_t *)&extraPatternBuffer,
                              (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin,
                              &failedAddress, &failedData);
            lock_release();
            if (status != kStatus_Success)
            {
                debug_printf("Error: flash verify failed at address: 0x%x\r\n", failedAddress);
                return status;
            }
        }
#endif // !BL_FLASH_VERIFY_DISABLE
    }
#endif // !(FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 8)

    return kStatus_Success;
}
#endif // !BL_MIN_PROFILE

// See memory.h for documentation on this function.
status_t flash_mem_erase(uint32_t address, uint32_t length)
{
    status_t status;

    lock_acquire();
    status = flash_erase(&g_bootloaderContext.flashState, address, length, kFlashEraseKey);
    lock_release();

#if !BL_FLASH_VERIFY_DISABLE
    if ((status == kStatus_Success) && (g_bootloaderContext.propertyInterface->store->verifyWrites))
    {
        lock_acquire();
        status = flash_verify_erase(&g_bootloaderContext.flashState, address, length,
                                    (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin);
        lock_release();
        if (status != kStatus_Success)
        {
            debug_printf("Error: flash_verify_erase failed\r\n");
            return status;
       }
    }
#endif // !BL_FLASH_VERIFY_DISABLE

    return status;
}

// See memory.h for documentation on this function.
status_t flash_mem_erase_all(void)
{
    status_t status = kStatus_Success;

    // Decompose the the flash erase all into two region erases.
#if BL_TARGET_FLASH

    reserved_region_t * reservedRegion =
        &g_bootloaderContext.propertyInterface->store->reservedRegions[kProperty_FlashReservedRegionIndex];
    bool isReservedRegionEmpty = (reservedRegion->startAddress == 0) && (reservedRegion->endAddress == 0);
    const uint32_t eraseSize = g_bootloaderContext.propertyInterface->store->flashSectorSize;

    if (g_bootloaderContext.propertyInterface->store->validateRegions && !isReservedRegionEmpty)
    {
        // Erase the initial unreserved region, if any.
        if (reservedRegion->startAddress  > 0)
        {
            uint32_t length = ALIGN_DOWN(reservedRegion->startAddress, eraseSize);
            if (length > 0)
            {
                status = flash_mem_erase(0, length);
            }
        }

        // Erase the final unreserved region, if any.
        if (status == kStatus_Success)
        {
            memory_map_entry_t * map = (memory_map_entry_t *)&g_bootloaderContext.memoryMap[kIndexFlashArray];
            uint32_t start = ALIGN_UP(reservedRegion->endAddress, eraseSize);
            if (start < map->endAddress)
            {
                status = flash_mem_erase(start, (map->endAddress + 1) - start);
            }
        }

        return status;
    }

#endif // BL_TARGET_FLASH

    // Do full erase and verify.

    lock_acquire();
    status = flash_erase_all(&g_bootloaderContext.flashState, kFlashEraseKey);
    lock_release();

#if !BL_FLASH_VERIFY_DISABLE
    if ((status == kStatus_Success) && (g_bootloaderContext.propertyInterface->store->verifyWrites))
    {
        lock_acquire();
        status = flash_verify_erase_all(&g_bootloaderContext.flashState,
                                        (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin);
        lock_release();
        if (status != kStatus_Success)
        {
            debug_printf("Error: flash_verify_erase_all failed\r\n");
            return status;
        }
    }
#endif // !BL_FLASH_VERIFY_DISABLE


#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
    if (status == kStatus_Success)
    {
        isFlashRegionUnlocked = true;
    }
#endif // FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL



    return status;
}

// See memory.h for documentation on this function.
status_t flash_mem_erase_all_unsecure(void)
{
    status_t status;

    lock_acquire();
    status = flash_erase_all_unsecure(&g_bootloaderContext.flashState, kFlashEraseKey);
    lock_release();

    return status;
}

#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
//! @brief check if a flash region is in an XA controlled region or contains an XA controlled region.
//         and try to open flash program state by calling verify_erase_all command if needed.
status_t flash_preprocess_execute_only_region(uint32_t address, uint32_t length, flash_execute_only_access_state_t *state)
{
    status_t status = kStatus_Success;
    *state = kFlashAccess_UnLimited;

    // If a target flash location is in an execute-only protected segment, these program commands are not
    // allowed unless a Read 1s All Blocks command is executed and returns with a pass code
    flash_execute_only_access_state_t access_state;

    status = flash_is_execute_only(&g_bootloaderContext.flashState, address, length,  &access_state);
    if (status != kStatus_Success)
    {
        return status;
    }

    *state = access_state;

    if ( (access_state != kFlashAccess_UnLimited) && (!isFlashRegionUnlocked) )
    {
        status = flash_verify_erase_all(&g_bootloaderContext.flashState,
                                        (flash_margin_value_t)g_bootloaderContext.propertyInterface->store->flashReadMargin);
        if (status != kStatus_Success)
        {
            return kStatus_FlashRegionExecuteOnly;
        }

        isFlashRegionUnlocked = true;
    }
    return status;
}
#endif  // FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
