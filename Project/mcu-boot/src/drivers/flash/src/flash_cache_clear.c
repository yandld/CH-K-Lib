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
#include "SSD_FTFx_Common.h"
#include "flash/flash.h"
#include "device/fsl_device_registers.h"
#include "fsl_platform_status.h"
#include "bootloader_common.h"
#include <string.h>

#if BL_TARGET_FLASH

//! @brief A static buffer used to hold flash_cache_clear_command()
#define  FLASH_CACHE_CLEAR_COMMAND_FUNCTION_MAX_SIZE  32
static uint8_t flashCacheClearCommand[FLASH_CACHE_CLEAR_COMMAND_FUNCTION_MAX_SIZE];
//! @brief A function pointer used to point to relocated flash_cache_clear_command()
#define FTFx_REG32_ACCESS_TYPE  volatile uint32_t *
static void (*callFlashCacheClearCommand)(FTFx_REG32_ACCESS_TYPE);

//! @brief Run flash cache clear command
//!
//! This function should be copied to RAM for execution to make sure that code works
//! properly even flash cache is disabled.
//! It is for flash-resident bootloader only, not technically required for ROM or
//! flashloader (RAM-resident bootloader).
void flash_cache_clear_command(FTFx_REG32_ACCESS_TYPE ftfx_reg)
{
#if FSL_FEATURE_FLASH_HAS_MCM_FLASH_CACHE_CONTROLS
    *ftfx_reg |= MCM_PLACR_CFCC_MASK;
#elif (defined(CPU_MK66FN2M0VLQ18) || defined (CPU_MK65FN2M0VMI18))
    *ftfx_reg = (*ftfx_reg & ~FMC_PFB01CR_CINV_WAY_MASK) | FMC_PFB01CR_CINV_WAY(~0);
#elif FSL_FEATURE_FLASH_HAS_FMC_FLASH_CACHE_CONTROLS
    *ftfx_reg = (*ftfx_reg & ~FMC_PFB0CR_CINV_WAY_MASK) | FMC_PFB0CR_CINV_WAY(~0);
#else
//    #error "Unknown flash cache controller"
#endif // FSL_FEATURE_FTFx_MCM_FLASH_CACHE_CONTROLS
}

//! @brief Be used for determining the size of flash_cache_clear_command()
//!
//! This function must be defined that lexically follows flash_cache_clear_command(),
//! so we can determine the size of flash_cache_clear_command() at runtime and not worry
//! about toolchain or code generation differences.
void flash_cache_clear_command_end(void)
{}

//! @brief Copy flash_cache_clear_command() to RAM
//!
//! This function copies the memory between flash_cache_clear_command() and flash_cache_clear_command_end()
//! into the buffer which is also means that copying flash_cache_clear_command() to RAM.
void copy_flash_cache_clear_command(void)
{
    // Calculate the valid length of flash_cache_clear_command() memory
    uint32_t funcLength = (uint32_t)flash_cache_clear_command_end - (uint32_t)flash_cache_clear_command;

    assert(funcLength <= FLASH_CACHE_CLEAR_COMMAND_FUNCTION_MAX_SIZE);

    // Since the value of ARM function pointer is always odd, but the real start address
    // of function memory should be even, that's why -1 and +1 operation exist.
    memcpy((void *)flashCacheClearCommand, (void *)((uint32_t)flash_cache_clear_command - 1), funcLength);
    callFlashCacheClearCommand = (void (*)(FTFx_REG32_ACCESS_TYPE))((uint32_t)flashCacheClearCommand + 1);
}
#endif // BL_TARGET_FLASH

////////////////////////////////////////////////////////////////////////////////
//!
//! @brief Flash Cache Clear
//!
//! This function is used to perform the cache clear to the flash.
////////////////////////////////////////////////////////////////////////////////
#pragma optimize=none
void flash_cache_clear(void)
{
#if BL_TARGET_FLASH
    // We pass the ftfx register address as a parameter to flash_cache_clear_comamnd() instead of using
    // pre-processed MACROs or a global variable in flash_cache_clear_comamnd()
    // to make sure that flash_cache_clear_command() will be compiled into position-independent code (PIC).
#if FSL_FEATURE_FLASH_HAS_MCM_FLASH_CACHE_CONTROLS
    callFlashCacheClearCommand((FTFx_REG32_ACCESS_TYPE)&MCM_PLACR);
#elif (defined(CPU_MK66FN2M0VLQ18) || defined (CPU_MK65FN2M0VMI18))
    callFlashCacheClearCommand((FTFx_REG32_ACCESS_TYPE)&FMC_PFB01CR);
#elif FSL_FEATURE_FLASH_HAS_FMC_FLASH_CACHE_CONTROLS
    callFlashCacheClearCommand((FTFx_REG32_ACCESS_TYPE)&FMC_PFB0CR);
#else
//    #error "Unknown flash cache controller"
#endif // FSL_FEATURE_FTFx_MCM_FLASH_CACHE_CONTROLS
#else
#if FSL_FEATURE_FLASH_HAS_MCM_FLASH_CACHE_CONTROLS
    MCM_BWR_PLACR_CFCC(MCM, 1);
#elif (defined(CPU_MK66FN2M0VLQ18) || defined (CPU_MK65FN2M0VMI18))
    FMC_BWR_PFB01CR_CINV_WAY(FMC, ~0);
#elif FSL_FEATURE_FLASH_HAS_FMC_FLASH_CACHE_CONTROLS
    FMC_BWR_PFB0CR_CINV_WAY(FMC, ~0);
#else
//    #error "Unknown flash cache controller"
#endif // FSL_FEATURE_FTFx_MCM_FLASH_CACHE_CONTROLS
#endif // BL_TARGET_FLASH
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

