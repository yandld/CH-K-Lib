/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SWDHOST_CM_H
#define SWDHOST_CM_H

#include <stdint.h>
 
 
#ifndef NULL
#define NULL 0
#endif

typedef enum {
    RESET_HOLD,              // Hold target in reset
    RESET_PROGRAM,           // Reset target and setup for flash programming.
    RESET_RUN,               // Reset target and run normally
    RESET_RUN_WITH_DEBUG,    // Reset target and run with debug enabled (required for semihost)
    NO_DEBUG,                // Disable debug on running target
    DEBUG                    // Enable debug on running target
} TARGET_RESET_STATE;

uint8_t swd_init(void);
uint8_t swd_read_dp(uint8_t adr, uint32_t *val);
uint8_t swd_write_dp(uint8_t adr, uint32_t val);
uint8_t swd_read_ap(uint32_t adr, uint32_t *val);
uint8_t swd_write_ap(uint32_t adr, uint32_t val);
uint8_t swd_read_memory(uint32_t address, uint8_t *data, uint32_t size);
uint8_t swd_write_memory(uint32_t address, uint8_t *data, uint32_t size);
void swd_set_target_reset(uint8_t asserted);
uint8_t swd_is_semihost_event(uint32_t *r0, uint32_t *r1);
uint8_t swd_semihost_restart(uint32_t r0);


uint8_t swd_set_target_state(TARGET_RESET_STATE state);

#endif
