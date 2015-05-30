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

#include <string.h>
#include "normal_memory.h"
#include "pattern_fill.h"
#include "sram_init.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

//! @brief Interface to simulator memory operations.
const memory_region_interface_t g_normalMemoryInterface = {
    .init = &normal_mem_init,
#if BL_FEATURE_READ_MEMORY_CMD
    .read = &normal_mem_read,
#endif
    .write = &normal_mem_write,
#if BL_FEATURE_FILL_MEMORY_CMD
    .fill = &normal_mem_fill,
#endif
    .flush = NULL,
    .erase = NULL
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

//! See normal_memory.h for documentation on this function.
status_t normal_mem_init(void)
{
    return sram_init();
}

status_t normal_mem_read(uint32_t address, uint32_t length, uint8_t * buffer)
{
    memcpy((void *)buffer, (void *)address, length);
    return kStatus_Success;
}

status_t normal_mem_write(uint32_t address, uint32_t length, const uint8_t * buffer)
{
    memcpy((void *)address, (void *)buffer, length);
    return kStatus_Success;
}

#if !BL_MIN_PROFILE
status_t normal_mem_fill(uint32_t address, uint32_t length, uint32_t pattern)
{
    // Allow multiword stores in the pattern fill.
    pattern_fill(address, pattern, length, true);

    return kStatus_Success;
}
#endif // !BL_MIN_PROFILE

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
