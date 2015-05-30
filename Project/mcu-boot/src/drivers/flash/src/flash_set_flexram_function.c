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

#include "SSD_FTFx_Common.h"
#include "flash/flash.h"
#include "device/fsl_device_registers.h"
#include "fsl_platform_status.h"
#include "bootloader_common.h"

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
#if (FSL_FEATURE_FLASH_HAS_SET_FLEXRAM_FUNCTION_CMD)
static inline status_t flasn_check_flexram_function_option_range(flash_flexram_function_option_t option)
{
    if ((option != kFlash_FlexRAM_Available_As_RAM) && (option != kFlash_FlexRAM_Available_For_EEPROM))
    {
        return kStatus_InvalidArgument;
    }

    return kStatus_Success;
}
#endif // #if (FSL_FEATURE_FLASH_HAS_SET_FLEXRAM_FUNCTION_CMD)

// See flash.h for documentation of this function.
status_t flash_set_flexram_function(flash_driver_t * driver, flash_flexram_function_option_t option)
{

#if (!FSL_FEATURE_FLASH_HAS_SET_FLEXRAM_FUNCTION_CMD)
    {
        return kStatus_FlashCommandNotSupported;
    }

#else
    if (driver == NULL)
    {
        return kStatus_InvalidArgument;
    }

    status_t status = flasn_check_flexram_function_option_range(option);
    if (status != kStatus_Success)
    {
        return status;
    }

     // preparing passing parameter to verify all block command
    HW_FTFx_FCCOBx_WR(FTFx_BASE, 0, FTFx_SET_FLEXRAM_FUNCTION);
    HW_FTFx_FCCOBx_WR(FTFx_BASE, 1, (uint8_t)option);

    // calling flash command sequence function to execute the command
    return flash_command_sequence();
#endif // FSL_FEATURE_FLASH_HAS_SET_FLEXRAM_FUNCTION_CMD
}




////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

