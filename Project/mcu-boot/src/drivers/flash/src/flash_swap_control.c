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

#include "SSD_FTFx_Common.h"
#include "flash/flash.h"
#include "fsl_platform_status.h"
#include "fsl_platform_types.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
enum
{
    kFlashConfig_Start = 0x400,
    kFlashConfig_End = 0x40F
};

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
#if (FSL_FEATURE_FLASH_HAS_PFLASH_BLOCK_SWAP)
status_t flash_check_swap_control_option(flash_swap_control_option_t option)
{
    if (  (option == kFlashSwap_IntializeSystem) || (option == kFlashSwap_SetInUpdateState) \
        ||(option == kFlashSwap_SetInCompleteState) || (option == kFlashSwap_ReportStatus))
    {
        return kStatus_Success;
    }

    return kStatus_InvalidArgument;
}
#endif // (FSL_FEATURE_FLASH_HAS_PFLASH_BLOCK_SWAP)


// See flash.h for documentation on this function.
status_t flash_swap_control(flash_driver_t * driver, uint32_t address, flash_swap_control_option_t option, flash_swap_info_t * returnInfo)
{
#if (!FSL_FEATURE_FLASH_HAS_PFLASH_BLOCK_SWAP)
    return kStatus_FlashCommandNotSupported;
#else

    if ((driver == NULL) || (returnInfo == NULL))
    {
        return kStatus_InvalidArgument;
    }

    if (address & (FSL_FEATURE_FLASH_PFLASH_SWAP_CONTROL_CMD_ADDRESS_ALIGMENT-1))
    {
        return kStatus_FlashAlignmentError;
    }

    // Make sure address provided is in the lower half of Program flash but not in the Flash Configuration Field
    if ((address >= driver->PFlashTotalSize/2) || (address >= kFlashConfig_Start && address <= kFlashConfig_End))
    {
        return kStatus_InvalidArgument;
    }

    // Check the option.
    status_t returnCode = flash_check_swap_control_option(option);
    if (returnCode)
    {
        return returnCode;
    }

    kFCCOBx[0] = address;
    kFCCOBx[1] = 0xFFFFFFFFul;
    HW_FTFx_FCCOBx_WR(FTFx_BASE, 0, FTFx_SWAP_CONTROL);
    HW_FTFx_FCCOBx_WR(FTFx_BASE, 4, (uint8_t)option);

    returnCode = flash_command_sequence();

    returnInfo->flashSwapState = (flash_swap_state_t)FTFx->FCCOB5;
    returnInfo->currentSwapBlockStatus = (flash_swap_block_status_t)FTFx->FCCOB6;
    returnInfo->nextSwapBlockStatus = (flash_swap_block_status_t)FTFx->FCCOB7;

   return returnCode;
#endif // FSL_FEATURE_FLASH_HAS_PFLASH_BLOCK_SWAP
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
