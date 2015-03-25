/**
  ******************************************************************************
  * @file    iflash.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   design for manley
  ******************************************************************************
  */
#ifndef __CH_LIB_IFLASH_H__
#define __CH_LIB_IFLASH_H__

#include <stdint.h>


#define SECTOR_SIZE        (2048)
#define DEV_SIZE            (0x40000)

//!< API declare
void IFLASH_Init(void);
uint8_t FLASH_WriteSector(uint32_t sectorNo, uint16_t count, uint8_t const *buffer);
uint8_t FLASH_EraseSector(uint32_t sectorNo);
uint8_t FLASH_ProgramWord(uint32_t sectorNo, const uint8_t *buffer);

#endif

