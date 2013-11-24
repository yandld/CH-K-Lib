#ifndef __CPUIDY_H__
#define __CPUIDY_H__

#include "common.h"

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
State_Type CPUIDY_GetPinCount(uint32_t *pinCount);
State_Type CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName, uint32_t *memSizeInKB);



#endif

