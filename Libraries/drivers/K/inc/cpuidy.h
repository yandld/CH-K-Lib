/**
  ******************************************************************************
  * @file    clock.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
	* @note    此文件为内部文件，用于获取芯片的出厂信息，少部分用户使用
  ******************************************************************************
  */
#ifndef __CPUIDY_H__
#define __CPUIDY_H__

#include <stdint.h>

typedef enum
{
    kPFlashSizeInKB,                 
    kDFlashSizeInKB,
    kFlexNVMSizeInKB,
    kEEPORMSizeInByte,
    kRAMSizeInKB,
    kMemNameCount,
}CPUIDY_MemSize_Type;


//!< API functions
char *CPUIDY_GetFamID(void);
void CPUIDY_GetPinCount(uint32_t *pinCount);
void CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName, uint32_t *memSizeInKB);
void CPUIDY_GetUID(uint32_t * UIDArray);


#endif

