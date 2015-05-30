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

#ifndef _FLASH_H_
#define _FLASH_H_

#include "fsl_platform_common.h"
#include "fsl_platform_status.h"
#include "device/fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>

//! @addtogroup flash
//! @{

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Version constants for flash driver API.
enum _flash_driver_version_constants
{
    kFlashDriver_Version_Name   = 'F',
    kFlashDriver_Version_Major  = 1,
    kFlashDriver_Version_Minor  = 2,
    kFlashDriver_Version_Bugfix = 2
};

//! @brief Flash driver status codes.
enum _flash_status
{
    kStatus_FlashSizeError = MAKE_STATUS(kStatusGroup_FlashDriver, 0),
    kStatus_FlashAlignmentError = MAKE_STATUS(kStatusGroup_FlashDriver, 1),
    kStatus_FlashAddressError = MAKE_STATUS(kStatusGroup_FlashDriver, 2),
    kStatus_FlashAccessError = MAKE_STATUS(kStatusGroup_FlashDriver, 3),
    kStatus_FlashProtectionViolation = MAKE_STATUS(kStatusGroup_FlashDriver, 4),
    kStatus_FlashCommandFailure = MAKE_STATUS(kStatusGroup_FlashDriver, 5),
    kStatus_FlashUnknownProperty = MAKE_STATUS(kStatusGroup_FlashDriver, 6),
    kStatus_FlashEraseKeyError = MAKE_STATUS(kStatusGroup_FlashDriver, 7),
    kStatus_FlashRegionExecuteOnly = MAKE_STATUS(kStatusGroup_FlashDriver, 8),
    kStatus_FlashCommandNotSupported = MAKE_STATUS(kStatusGroup_FlashDriver, 15),
};

//! @brief Enumeration for flash driver API keys.
enum _flash_driver_api_keys
{
    //! @brief Key value used to validate all flash erase APIs.
    kFlashEraseKey = FOUR_CHAR_CODE('k', 'f', 'e', 'k')
};

//! @brief Enumeration for supported flash margin levels.
typedef enum _flash_margin_value {
    kFlashMargin_Normal,
    kFlashMargin_User,
    kFlashMargin_Factory,

    // Not real margin level, Used to determine the range of valid margin level.
    kFlashMargin_Invalid
} flash_margin_value_t;

//! @brief Enumeration for the three possible flash security states.
typedef enum _flash_security_state {
    kFlashNotSecure,
    kFlashSecureBackdoorEnabled,
    kFlashSecureBackdoorDisabled
} flash_security_state_t;

//! @brief Enumeration for the three possible flash protection levels.
typedef enum _flash_protection_state {
    kFlashProtection_Unprotected,
    kFlashProtection_Protected,
    kFlashProtection_Mixed
} flash_protection_state_t;


//! @brief Enumeration for the three possible flash execute access levels.
typedef enum _flash_execute_only_access_state {
    kFlashAccess_UnLimited,
    kFlashAccess_ExecuteOnly,
    kFlashAccess_Mixed
} flash_execute_only_access_state_t;

//! @brief Enumeration for various flash properties.
typedef enum _flash_property {
    kFlashProperty_SectorSize,
    kFlashProperty_TotalFlashSize,
    kFlashProperty_BlockSize,
    kFlashProperty_BlockCount,
    kFlashProperty_FlashBlockBaseAddr,
    kFlashProperty_FlashFacSupport,
    kFlashProperty_FlashAccessSegmentSize,
    kFlashProperty_FlashAccessSegmentCount,
} flash_property_t;


//! @brief callback type used for pflash block
typedef void (* flash_callback_t)(void);


//! @brief Flash driver state information.
//!
//! An instance of this structure is allocated by the user of the flash driver and
//! passed into each of the driver APIs.
typedef struct _flash_driver {
    uint32_t PFlashBlockBase;           //!< Base address of the first PFlash block
    uint32_t PFlashTotalSize;           //!< Size of all combined PFlash block.
    uint32_t PFlashBlockCount;          //!< Number of PFlash blocks.
    uint32_t PFlashSectorSize;          //!< Size in bytes of a sector of PFlash.
    flash_callback_t PFlashCallback;    //!< Callback function for flash API.
    uint32_t PFlashAccessSegmentSize;   //!< Size in bytes of a access segment of PFlash.
    uint32_t PFlashAccessSegmentCount;  //!< Number of PFlash access segments.
} flash_driver_t;


//! @brief Enumeration for the two possible options of flash read resource command.
typedef enum _flash_read_resource_option
{
    kFlashResource_ProgramIFR = 0,
    kFlashResource_VersionID = 1
} flash_read_resource_option_t;


//! @brief Enumeration for the two possilbe options of set flexram function command.
typedef enum _flash_flexram_fcunction_option
{
    kFlash_FlexRAM_Available_As_RAM     = 0xFF,  //!< Option used to make FlexRAM available as RAM
    kFlash_FlexRAM_Available_For_EEPROM = 0x00   //!< Option used to make FlexRAM available for EEPROM
}flash_flexram_function_option_t;


//! @brief Enumeration for the possible options of Swap Control commands
typedef enum _flash_swap_control_option
{
    kFlashSwap_IntializeSystem      = 1,    //!< Option used to Intialize Swap System
    kFlashSwap_SetInUpdateState     = 2,    //!< Option used to Set Swap in Update State
    kFlashSwap_SetInCompleteState   = 4,    //!< Option used to Set Swap in Complete State
    kFlashSwap_ReportStatus         = 8     //!< Option used to Report Swap Status
}flash_swap_control_option_t;


//! @brief Enumeration for the possible flash swap status;
typedef enum _flash_swap_state
{
    kFlashSwapState_Uninitialized  = 0,
    kFlashSwapState_Ready          = 1,
    kFlashSwapState_Update         = 2,
    kFlashSwapState_UpdateErased   = 3,
    kFlashSwapState_Complete       = 4,
}flash_swap_state_t;


//! @breif Enumeration for the possible flash swap block status
typedef enum _flash_swap_block_status
{
    kFlashSwapBlockStatus_LowerHalfProgramBlocksAtZero = 0,
    kFlashSwapBlockStatus_UpperHalfProgramBlocksAtZero = 1,
}flash_swap_block_status_t;


//! @brief Flash Swap information.
typedef struct  _flash_swap_info
{
    flash_swap_state_t flashSwapState;                  //!<
    flash_swap_block_status_t currentSwapBlockStatus;
    flash_swap_block_status_t nextSwapBlockStatus;
}flash_swap_info_t;



////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

//! @name Initialization
//@{

/*!
 * @brief Initializes global flash properties structure members
 *
 * This function checks and initializes Flash module for the other Flash APIs.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @return An error code or kStatus_Success
 */
status_t flash_init(flash_driver_t * driver);

/*!
 * @brief Register the desired flash callback function
 * @param driver    Pointer to storage for the driver runtime state.
 * @param callback  callback function to be stored in driver
 *
 * @return An error code or kStatus_Success
 */
status_t flash_register_callback(flash_driver_t * driver, flash_callback_t callback);

//@}

//! @name Erasing
//@{

/*!
 * @brief Erases entire flash
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param key value used to validate all flash erase APIs.
 * @return An error code or kStatus_Success
 */
status_t flash_erase_all(flash_driver_t * driver, uint32_t key);

/*!
 * @brief Erases flash sectors encompassed by parameters passed into function
 *
 * This function erases the appropriate number of flash sectors based on the
 * desired start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be erased.
 *        The start address does not need to be sector aligned but must be word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be erased. Must be word aligned.
 * @param key value used to validate all flash erase APIs.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_erase(flash_driver_t * driver, uint32_t start, uint32_t lengthInBytes, uint32_t key);

/*!
 * @brief Erases entire flash, including protected sectors.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param key value used to validate all flash erase APIs.
 * @return An error code or kStatus_Success
 */
status_t flash_erase_all_unsecure(flash_driver_t * driver, uint32_t key);

//@}

//! @name Programming
//@{

/*!
 * @brief Programs flash with data at locations passed in through parameters
 *
 * This function programs the flash memory with desired data for a given
 * flash area as determined by the start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be programmed. Must be
 *              word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be programmed. Must be word-aligned.
 * @param src Pointer to the source buffer of data that is to be programmed
 *        into the flash.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_program(flash_driver_t * driver, uint32_t start, uint32_t * src, uint32_t lengthInBytes);

/*!
 * @brief Programs Program Once Field through parameters
 *
 * This function programs the Program Once Field with desired data for a given
 * flash area as determined by the index and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param index The index indicating which area of Program Once Field to be programmed.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be programmed. Must be word-aligned.
 * @param src Pointer to the source buffer of data that is to be programmed
 *        into the Program Once Field.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_program_once(flash_driver_t * driver, uint32_t index, uint32_t * src, uint32_t lengthInBytes);

/*!
 * @brief Programs flash with data at locations passed in through parameters via Program Section command
 *
 * This function programs the flash memory with desired data for a given
 * flash area as determined by the start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be programmed. Must be
 *              word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be programmed. Must be word-aligned.
 * @param src Pointer to the source buffer of data that is to be programmed
 *        into the flash.
 *
 * @return An error code or kStatus_Success
 */

status_t flash_program_section(flash_driver_t * driver, uint32_t start, uint32_t * src, uint32_t lengthInBytes);

//@}

//! @name Reading
//@{

/*!
 * @brief Read resource with data at locations passed in through parameters
 *
 * This function reads the flash memory with desired location for a given
 * flash area as determined by the start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be programmed. Must be
 *              word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be read. Must be word-aligned.
 * @param dst Pointer to the destination buffer of data that is used to store
 *        data to be read.
 * @param option The resource option which indicates which area should be read back.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_read_resource(flash_driver_t * driver, uint32_t start, uint32_t *dst, uint32_t lengthInBytes, flash_read_resource_option_t option);

/*!
 * @brief Read Program Once Field through parameters
 *
 * This function reads the read once feild with given index and length
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param index The index indicating the area of program once field to be read.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be programmed. Must be word-aligned.
 * @param dst Pointer to the destination buffer of data that is used to store
 *        data to be read.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_read_once(flash_driver_t * driver, uint32_t index, uint32_t *dst, uint32_t lengthInBytes);

//@}



//! @name Security
//@{

/*!
 * @brief Returns the security state via the pointer passed into the function
 *
 * This function retrieves the current Flash security status, including the
 * security enabling state and the backdoor key enabling state.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param state Pointer to the value returned for the current security status code:
 *          - #kFlashNotSecure
 *          - #kFlashSecureBackdoorEnabled
 *          - #kFlashSecureBackdoorDisabled
 *
 * @return kStatus_Success
 */
status_t flash_get_security_state(flash_driver_t * driver, flash_security_state_t * state);

/*!
 * @brief Allows user to bypass security with a backdoor key
 *
 * If the MCU is in secured state, this function will unsecure the MCU by
 * comparing the provided backdoor key with ones in the Flash Configuration
 * Field.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param backdoorKey Pointer to the user buffer containing the backdoor key.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_security_bypass(flash_driver_t * driver, const uint8_t * backdoorKey);

//@}

//! @name Verification
//@{

/*!
 * @brief Verifies erasure of entire flash at specified margin level
 *
 * This function will check to see if the flash have been erased to the
 * specified read margin level.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param margin Read margin choice as follows: #kFlashMargin_Normal,
 *        #kFlashMargin_User, or #kFlashMargin_Factory
 *
 * @return An error code or kStatus_Success
 */
status_t flash_verify_erase_all(flash_driver_t * driver, flash_margin_value_t margin);

/*!
 * @brief Verifies erasure of desired flash area at specified margin level
 *
 * This function will check the appropriate number of flash sectors based on
 * the desired start address and length to see if the flash have been erased
 * to the specified read margin level.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be verified.
 *        The start address does not need to be sector aligned but must be word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be verified. Must be word-aligned.
 * @param margin Read margin choice as follows: #kFlashMargin_Normal,
 *        #kFlashMargin_User, or #kFlashMargin_Factory
 *
 * @return An error code or kStatus_Success
 */
status_t flash_verify_erase(flash_driver_t * driver, uint32_t start, uint32_t lengthInBytes, flash_margin_value_t margin);

/*!
 * @brief Verifies programming of desired flash area at specified margin level
 *
 * This function verifies the data programed in the flash memory using the
 * Flash Program Check Command and compares it with expected data for a given
 * flash area as determined by the start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be verified. Must be word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be verified. Must be word-aligned.
 * @param expectedData Pointer to the expected data that is to be
 *        verified against.
 * @param margin Read margin choice as follows: kFlashMargin_User or
 *        kFlashMargin_Factory
 * @param failedAddress Pointer to returned failing address.
 * @param failedData Pointer to returned failing data.  Some derivitives do
 *        not included failed data as part of the FCCOBx registers.  In this
 *        case, zeros are returned upon failure.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_verify_program(flash_driver_t * driver, uint32_t start, uint32_t lengthInBytes,
                              const uint8_t * expectedData, flash_margin_value_t margin,
                              uint32_t * failedAddress, uint8_t * failedData);

//@}

//! @name Protection
//@{

/*!
 * @brief Returns the protection state of desired flash area via the pointer passed into the function
 *
 * This function retrieves the current Flash protect status for a given
 * flash area as determined by the start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be checked. Must be word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be checked.  Must be word-aligned.
 * @param protection_state Pointer to the value returned for the current
 *        protection status code for the desired flash area.  The status code
 *        can be kFlashProtection_Unprotected, kFlashProtection_Protected, or
 *        kFlashProtection_Mixed.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_is_protected(flash_driver_t * driver, uint32_t start, uint32_t lengthInBytes, flash_protection_state_t * protection_state);


/*!
 * @brief Returns the access state of desired flash area via the pointer passed into the function
 *
 * This function retrieves the current Flash access status for a given
 * flash area as determined by the start address and length.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param start The start address of the desired flash memory to be checked. Must be word-aligned.
 * @param lengthInBytes The length, given in bytes (not words or long-words)
 *        to be checked.  Must be word-aligned.
 * @param access_state Pointer to the value returned for the current
 *        access status code for the desired flash area.  The status code
 *        can be kFlashAccess_UnLimited, kFlashAccess_ExecuteOnly, or
 *        kFlashAccess_Mixed.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_is_execute_only(flash_driver_t * driver, uint32_t start, uint32_t lengthInBytes, flash_execute_only_access_state_t * access_state);

//@}

//! @name Properties
//@{

/*!
 * @brief Returns the desired flash property.
 *
 * @param driver Pointer to storage for the driver runtime state.
 * @param whichProperty The desired property from the list of properties in
 *        enum flash_property_t
 * @param value Pointer to the value returned for the desired flash property
 *
 * @return An error code or kStatus_Success
 */
status_t flash_get_property(flash_driver_t * driver, flash_property_t whichProperty, uint32_t * value);

//@}

//! @name FlexRAM
//@{

/*!
 * @brief Set FlexRAM Function command
 * @param driver Pointer to storage for the driver runtime state.
 * @param option The option used to set work mode of FlexRAM
 *
 * @return An error code or kStatus_Success
 */
status_t flash_set_flexram_function(flash_driver_t * driver, flash_flexram_function_option_t option);

//@}


//! @name Swap
//@{

/*!
 * @brief Configure Swap function or Check the swap state of Flash Module
 * @param driver Pointer to storage for the driver runtime state.
 * @param address Address used to configure the flash swap function
 * @param option The possible option used to configure Flash Swap function or check the flash swap status
 * param returnInfo Pointer to the data which is used to return the information of flash swap.
 *
 * @return An error code or kStatus_Success
 */
status_t flash_swap_control(flash_driver_t * driver, uint32_t address, flash_swap_control_option_t option, flash_swap_info_t * returnInfo);

//@}

//@}

#if defined(__cplusplus)
}
#endif

//! @}

#endif // _FLASH_H_
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
