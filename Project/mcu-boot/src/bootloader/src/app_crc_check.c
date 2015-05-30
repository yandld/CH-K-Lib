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
#include <string.h>
#include "bootloader_common.h"
#include "property/property.h"
#include "memory/memory.h"
#include "bootloader/context.h"
#include "crc/crc32.h"
#include "bootloader/app_crc_check.h"
#include "utilities/vector_table_info.h"

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

// See app_crc_check.h for documentation on this function.
void init_crc_check_status(property_store_t * propertyStore)
{
    // Crc check is available if BCA exists and crc parameters are set (not all 0xff bytes)
    if ( !((propertyStore->configurationData.crcStartAddress == 0xffffffff) &&
           (propertyStore->configurationData.crcByteCount == 0xffffffff) &&
           (propertyStore->configurationData.crcExpectedValue == 0xffffffff)) )
    {
        propertyStore->crcCheckStatus = kStatus_AppCrcCheckInactive;
    }
    else
    {
        propertyStore->crcCheckStatus = kStatus_AppCrcCheckInvalid;
    }
}

//! @brief Calculate crc on a range of flash, specified in the bootloader configuration area.
static uint32_t calculate_application_crc32(bootloader_configuration_data_t * config)
{
    uint32_t crc32;

    // Initialize the CRC32 information
    crc32_data_t crcInfo;
    crc32_init(&crcInfo);

    // Run CRC, Considering skip crcExpectedValue address
    uint32_t bypassStartAddress = kBootloaderConfigAreaAddress + ((uint32_t)&config->crcExpectedValue - (uint32_t)&config->tag);
    uint32_t bypassEndAddress = bypassStartAddress + sizeof(config->crcExpectedValue);

    if ((config->crcStartAddress >= bypassEndAddress) || (config->crcStartAddress + config->crcByteCount <= bypassStartAddress))
    {
        crc32_update(&crcInfo, (uint8_t *)config->crcStartAddress, config->crcByteCount);
    }
    else
    {
        // Assume that crcExpectedValue address (4 byte) resides in crc addresses completely
        crc32_update(&crcInfo, (uint8_t *)config->crcStartAddress, bypassStartAddress - config->crcStartAddress);
        crc32_update(&crcInfo, (uint8_t *)bypassEndAddress, config->crcStartAddress + config->crcByteCount - bypassEndAddress);
    }

    // Finalize the CRC calculations
    crc32_finalize(&crcInfo, &crc32);

    return crc32;
}

//! @brief Check if crc check addresses reside in the range of valid memory space (Pflash or QSPIflash)
static bool is_crc_check_address_valid(bootloader_configuration_data_t * config)
{
    bool isAddressValid;

    // Check if crc byte count is not equal to zero and crc end address does't exceed 4G address space
    isAddressValid = (config->crcByteCount != 0) && (config->crcStartAddress <= 0xffffffff - config->crcByteCount + 1);

    // Check if addresses reside in internal flash
    const memory_map_entry_t* map = (const memory_map_entry_t *)&g_bootloaderContext.memoryMap[kIndexFlashArray];
    if (isAddressValid)
    {
        isAddressValid = ( (config->crcStartAddress >= map->startAddress) &&
                           (config->crcStartAddress + config->crcByteCount - 1 <= map->endAddress) );
#if BL_HAS_QSPI_MODULE
        // Check if addresses reside in external QSPI flash
        if ((!isAddressValid) && is_qspi_present())
        {
            map = (const memory_map_entry_t *)&g_bootloaderContext.memoryMap[kIndexQspiMemory];

            isAddressValid = ( (config->crcStartAddress >=  map->startAddress) &&
                               (config->crcStartAddress + config->crcByteCount - 1 <= map->endAddress) );
        }
#endif // BL_HAS_QSPI_MODULE
    }

    return isAddressValid;
}

// See app_crc_check.h for documentation on this function.
bool is_application_crc_check_pass(void)
{
    bool isCrcCheckPassed = true;

    property_store_t * propertyStore = g_bootloaderContext.propertyInterface->store;
    if (kStatus_AppCrcCheckInvalid != propertyStore->crcCheckStatus)
    {
        isCrcCheckPassed = false;

        // Check if crc check addresses reside in the range of valid memory space (Pflash or QSPIflash)
        if (is_crc_check_address_valid(&propertyStore->configurationData))
        {
            uint32_t calculated_crc = calculate_application_crc32(&propertyStore->configurationData);

            if (calculated_crc != propertyStore->configurationData.crcExpectedValue)
            {
                propertyStore->crcCheckStatus = kStatus_AppCrcCheckFailed;
            }
            else
            {
                isCrcCheckPassed = true;
                propertyStore->crcCheckStatus = kStatus_AppCrcCheckPassed;
            }
        }
        else
        {
            propertyStore->crcCheckStatus = kStatus_AppCrcCheckOutOfRange;
        }

        if (!isCrcCheckPassed)
        {
            assert_pin_to_indicate_crc_check_failure();
        }
    }

    return isCrcCheckPassed;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
