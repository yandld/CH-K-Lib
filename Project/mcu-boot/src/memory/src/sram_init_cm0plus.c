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

#include "bootloader_common.h"
#include "bootloader/context.h"
#include "memory/memory.h"
#if !defined(BOOTLOADER_HOST)
#include "device/fsl_device_registers.h"
#endif // BOOTLOADER_HOST
#include "utilities/fsl_assert.h"
#include "sram_init.h"


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////



//! @brief Initialize address ranges of SRAM for chips belongs to cm0plus family
status_t sram_init(void)
{
#if defined (__CORE_CM0PLUS_H_GENERIC)

    uint32_t tmp = HW_SIM_SDID(SIM_BASE).B.SRAMSIZE;

    uint32_t ram_size = 0;
    if (tmp <= kMaxRamIndex)
    {
        ram_size = kMinKlRamSize << tmp;
    }

    assert(ram_size > 0);

    if (ram_size > 0)
    {
        // Update address range of SRAM
        memory_map_entry_t * map = (memory_map_entry_t *)&g_bootloaderContext.memoryMap[kIndexSRAM];
        tmp = ram_size / (kSram_LowerPart + kSram_UpperPart);
        map->startAddress = kSRAMSeparatrix - tmp * kSram_LowerPart;          // start of SRAM
        map->endAddress = kSRAMSeparatrix + tmp * kSram_UpperPart - 1;        // end of SRAM
    }
#else
    #error "This function only applies to cm0plus family"
#endif // __CORE_CM0PLUS_H_GENERIC

    return kStatus_Success;
}



////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
