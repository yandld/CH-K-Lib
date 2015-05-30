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

#include "bootloader_common.h"
#include "property/property.h"
#include "memory/memory.h"
#include "packet/command_packet.h"
#include "packet/serial_packet.h"
#include "bootloader/peripheral.h"
#include "bootloader/context.h"
#include "bootloader/bl_version.h"
#include "utilities/fsl_assert.h"
#include <string.h>
#include "flash/flash.h"
#include "device/fsl_device_registers.h"
#if BL_HAS_QSPI_MODULE
#include "qspi/qspi.h"
#endif // BL_HAS_QSPI_MODULE
#if BL_ENABLE_CRC_CHECK
#include "bootloader/app_crc_check.h"
#endif // BL_ENABLE_CRC_CHECK

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

#if (defined(__ICCARM__))
#pragma section = ".intvec"
#pragma section = "ApplicationFlash"
#pragma section = "ApplicationRam"
#endif // __ICCARM__

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

//! @brief Storage for property values.
property_store_t g_propertyStore;

// See property.h for documentation on this data structure.
const property_interface_t g_propertyInterface = {
#if BL_FEATURE_BOOTLOADER_CONFIG_AREA
    .load_user_config = bootloader_property_load_user_config,
#endif
    .init = bootloader_property_init,
    .get = bootloader_property_get,
    .set_uint32 = bootloader_property_set_uint32,
    .store = &g_propertyStore
};

//! @brief Storage for property values computed every time they are read.
static uint32_t s_propertyReturnValue;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

// See property.h for documentation on this function.
status_t bootloader_property_load_user_config(void)
{

    bootloader_configuration_data_t * config = &g_bootloaderContext.propertyInterface->store->configurationData;

    // Copy bootloader configuration data from the flash into the property store.
    memcpy(config,
        (const void *)kBootloaderConfigAreaAddress,
        sizeof(bootloader_configuration_data_t));

    // Verify tag. If it is invalid, wipe the config data to all 0xff.
    if (kPropertyStoreTag != config->tag)
    {
        memset(config, 0xff, sizeof(bootloader_configuration_data_t));
    }

    // Update available peripherals based on specific chips
    update_available_peripherals();

    return kStatus_Success;
}

// See property.h for documentation on this function.
status_t bootloader_property_init(void)
{
    property_store_t * propertyStore = g_bootloaderContext.propertyInterface->store;
#if !BL_FEATURE_BOOTLOADER_CONFIG_AREA
    bootloader_configuration_data_t * config = &g_bootloaderContext.propertyInterface->store->configurationData;
    memset(config, 0xff, sizeof(bootloader_configuration_data_t));
#endif

    // Fill in default values.
    propertyStore->bootloaderVersion.name = kBootloader_Version_Name;
    propertyStore->bootloaderVersion.major = kBootloader_Version_Major;
    propertyStore->bootloaderVersion.minor = kBootloader_Version_Minor;
    propertyStore->bootloaderVersion.bugfix = kBootloader_Version_Bugfix;

    propertyStore->serialProtocolVersion.name = kSerialProtocol_Version_Name;
    propertyStore->serialProtocolVersion.major = kSerialProtocol_Version_Major;
    propertyStore->serialProtocolVersion.minor = kSerialProtocol_Version_Minor;
    propertyStore->serialProtocolVersion.bugfix = kSerialProtocol_Version_Bugfix;

    propertyStore->verifyWrites = true;
    propertyStore->validateRegions = true;

    propertyStore->availableCommands = kAvailableCommands;

    /// Initialize flash properties.
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_FlashBlockBaseAddr, &propertyStore->flashStartAddress);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_TotalFlashSize, &propertyStore->flashSizeInBytes);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_SectorSize, &propertyStore->flashSectorSize);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_BlockSize, &propertyStore->flashBlockSize);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_BlockCount, &propertyStore->flashBlockCount);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_FlashFacSupport, &propertyStore->flashFacSupport);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_FlashAccessSegmentSize, &propertyStore->flashAccessSegmentSize);
    flash_get_property(&g_bootloaderContext.flashState, kFlashProperty_FlashAccessSegmentCount, &propertyStore->flashAccessSegmentCount);

    // Fill in reserved regions.
    //! @todo Support other tool chain

    uint32_t flashStart = 0;
    uint32_t flashEnd = 0;
    uint32_t ramStart = 0;
    uint32_t ramEnd = 0;

#if (defined(__ICCARM__))

#if !BL_TARGET_FLASH
    flashStart = 0;
    flashEnd = 0;
#else
    flashStart = (uint32_t)__section_begin(".intvec");
    flashEnd = (uint32_t)__section_end("ApplicationFlash");
    assert(flashEnd);

    // Force flash erase size alignment.
    flashStart = ALIGN_DOWN(flashStart, propertyStore->flashSectorSize);
    flashEnd = ALIGN_UP(flashEnd, propertyStore->flashSectorSize) - 1;
#endif

#if defined(BL_TARGET_RAM)
    // Include all of text and data in RAM reserved region
    ramStart = (uint32_t)__section_begin(".intvec");
#else
    ramStart = (uint32_t)__section_begin("ApplicationRam");
#endif // BL_TARGET_RAM
    ramEnd = (uint32_t)__section_end("ApplicationRam") - 1;
    assert(ramEnd);

#endif // __ICCARM__

    propertyStore->reservedRegions[kProperty_FlashReservedRegionIndex].startAddress = flashStart;
    propertyStore->reservedRegions[kProperty_FlashReservedRegionIndex].endAddress = flashEnd;
    propertyStore->reservedRegions[kProperty_RamReservedRegionIndex].startAddress = ramStart;
    propertyStore->reservedRegions[kProperty_RamReservedRegionIndex].endAddress = ramEnd;

    // Fill in available peripherals array.
    const peripheral_descriptor_t * peripherals = g_bootloaderContext.allPeripherals;
    propertyStore->availablePeripherals = 0;
    for (uint32_t i = 0; peripherals[i].typeMask != 0; ++i)
    {
        // Check that the peripheral is enabled in the user configuration data.
        if (propertyStore->configurationData.enabledPeripherals & peripherals[i].typeMask)
        {
            propertyStore->availablePeripherals |= peripherals[i].typeMask;
        }
    }

    // Fill in unique device id value.
    // Different series have different length of UID (K series=128 bits, KL series=80 bits)
#if defined(SIM_UIDH)
    propertyStore->UniqueDeviceId.uidh = SIM_RD_UIDH(SIM);
#endif
    propertyStore->UniqueDeviceId.uidmh = SIM_RD_UIDMH(SIM);
    propertyStore->UniqueDeviceId.uidml = SIM_RD_UIDML(SIM);
    propertyStore->UniqueDeviceId.uidl = SIM_RD_UIDL(SIM);

    // Set address range of RAM in property interface
    const memory_map_entry_t * map = (memory_map_entry_t *)&g_bootloaderContext.memoryMap[kIndexSRAM];
    propertyStore->ramStartAddress = map->startAddress;
    propertyStore->ramSizeInBytes = map->endAddress - map->startAddress + 1;

#if BL_ENABLE_CRC_CHECK
    // Initialize crc check status property based on BCA related fields.
    init_crc_check_status(propertyStore);
#endif

    // Fill in default margin level.
    propertyStore->flashReadMargin = (uint32_t)kFlashMargin_User;

#if BL_HAS_QSPI_MODULE
    propertyStore->qspiInitStatus = get_qspi_otfad_init_status();
#endif // BL_HAS_QSPI_MODULE


    return kStatus_Success;
}

// See property.h for documentation on this function.
status_t bootloader_property_get(uint8_t tag, const void ** value, uint32_t * valueSize)
{
    property_store_t * propertyStore = g_bootloaderContext.propertyInterface->store;

    // Set default value size, may be modified below.
    uint32_t returnSize = sizeof(uint32_t);
    const void * returnValue;
    switch (tag)
    {
    case kPropertyTag_BootloaderVersion:
        returnValue = &propertyStore->bootloaderVersion.version;
        break;
        
    case kPropertyTag_AvailablePeripherals:
        returnValue = &propertyStore->availablePeripherals;
        break;
        
    case kPropertyTag_FlashStartAddress:
        returnValue = &propertyStore->flashStartAddress;
        break;
        
    case kPropertyTag_FlashSizeInBytes:
        returnValue = &propertyStore->flashSizeInBytes;
        break;
        
    case kPropertyTag_FlashSectorSize:
        returnValue = &propertyStore->flashSectorSize;
        break;
        
    case kPropertyTag_FlashBlockCount:
        returnValue = &propertyStore->flashBlockCount;
        break;
        
    case kPropertyTag_RAMStartAddress:
        returnValue = &propertyStore->ramStartAddress;
        break;
        
    case kPropertyTag_RAMSizeInBytes:
        returnValue = &propertyStore->ramSizeInBytes;
        break;
        
    case kPropertyTag_AvailableCommands:
        returnValue = &propertyStore->availableCommands;
        break;
        
#if BL_ENABLE_CRC_CHECK
    case kPropertyTag_CrcCheckStatus:
        returnValue = &propertyStore->crcCheckStatus;
        break;
#endif //else falls through to unknown
        
    case kPropertyTag_VerifyWrites:
        returnValue = &propertyStore->verifyWrites;
        break;
        
    case kPropertyTag_ValidateRegions:
        returnValue = &propertyStore->validateRegions;
        break;
        
    case kPropertyTag_MaxPacketSize:
        // Read the max packet size from the active peripheral.
        s_propertyReturnValue = g_bootloaderContext.activePeripheral->packetInterface->getMaxPacketSize(g_bootloaderContext.activePeripheral);
        returnValue = &s_propertyReturnValue;
        break;
        
    case kPropertyTag_ReservedRegions:
        returnSize = sizeof(propertyStore->reservedRegions);
        returnValue = propertyStore->reservedRegions;
        break;
        
    case kPropertyTag_SystemDeviceId:
        // Get system device identification
        s_propertyReturnValue = SIM_RD_SDID(SIM);
        returnValue = &s_propertyReturnValue;
        break;
        
    case kPropertyTag_FlashSecurityState:
        {
            flash_security_state_t securityState;
            flash_get_security_state(&g_bootloaderContext.flashState, &securityState);
            s_propertyReturnValue = (securityState != kFlashNotSecure);
            returnValue = &s_propertyReturnValue;
            break;
        }
        
    case kPropertyTag_UniqueDeviceId:
        returnSize = sizeof(propertyStore->UniqueDeviceId);
        returnValue = &propertyStore->UniqueDeviceId;
        break;

#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL        
    case kPropertyTag_FacSupport:
        returnSize = sizeof(propertyStore->flashFacSupport);
        returnValue = &propertyStore->flashFacSupport;
        break;
        
    case kPropertyTag_FlashAccessSegmentSize:
        returnSize = sizeof(propertyStore->flashAccessSegmentSize);
        returnValue = &propertyStore->flashAccessSegmentSize;
        break;
        
    case kPropertyTag_FlashAccessSegmentCount:
        returnSize = sizeof(propertyStore->flashAccessSegmentCount);
        returnValue = &propertyStore->flashAccessSegmentCount;
        break;
#endif
        
    case kPropertyTag_FlashReadMargin:
        returnSize = sizeof(propertyStore->flashReadMargin);
        returnValue = &propertyStore->flashReadMargin;
        break;
        
#if BL_HAS_QSPI_MODULE
    case kPropertyTag_QspiInitStatus:
        returnValue = &propertyStore->qspiInitStatus;
        break;
#endif //else falls through to unknown
        
    default:
        return kStatus_UnknownProperty;
    }

    // Set the return size.
    if (valueSize)
    {
        *valueSize = returnSize;
    }

    // Set the return value
    if (value)
    {
        *value = returnValue;
    }

    return kStatus_Success;
}

// See property.h for documentation on this function.
status_t bootloader_property_set_uint32(uint8_t tag, uint32_t value)
{
    property_store_t * propertyStore = g_bootloaderContext.propertyInterface->store;

    switch (tag)
    {
    case kPropertyTag_VerifyWrites:
        if (value != 0 && value != 1)
        {
            return kStatus_InvalidPropertyValue;
        }
        propertyStore->verifyWrites = value;
        return kStatus_Success;
        
    case kPropertyTag_FlashReadMargin:
        if (value >= kFlashMargin_Invalid)
        {
            return kStatus_InvalidPropertyValue;
        }
        propertyStore->flashReadMargin = value;
        return kStatus_Success;
        
    case kPropertyTag_ValidateRegions:   
    case kPropertyTag_BootloaderVersion:
    case kPropertyTag_AvailablePeripherals:
    case kPropertyTag_FlashStartAddress:
    case kPropertyTag_FlashSizeInBytes:
    case kPropertyTag_FlashSectorSize:
    case kPropertyTag_FlashBlockCount:
    case kPropertyTag_RAMStartAddress:
    case kPropertyTag_RAMSizeInBytes:
    case kPropertyTag_AvailableCommands:
#if BL_ENABLE_CRC_CHECK
    case kPropertyTag_CrcCheckStatus:
#endif
#if BL_HAS_QSPI_MODULE
    case kPropertyTag_QspiInitStatus:
#endif
    case kPropertyTag_MaxPacketSize:
    case kPropertyTag_ReservedRegions:
    case kPropertyTag_SystemDeviceId:
    case kPropertyTag_FlashSecurityState:
#if FSL_FEATURE_FLASH_HAS_ACCESS_CONTROL
    case kPropertyTag_FacSupport:
    case kPropertyTag_FlashAccessSegmentSize:
    case kPropertyTag_FlashAccessSegmentCount:
#endif        

        return kStatus_ReadOnlyProperty;
    default:
        return kStatus_UnknownProperty;
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

