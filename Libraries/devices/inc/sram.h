#ifndef __SRAM_H__
#define __SRAM_H__
#include "flexbus.h"

#define SDRAM_ADDRESS_BASE   (0x70000000)
#define SRAM_SIZE            (512*1024)

#define SRAM_START_ADDRESS	(volatile uint8_t *)(SDRAM_ADDRESS_BASE)

//!< API functions
void SRAM_Init(void);
uint32_t SRAM_SelfTest(void);

#endif