#include "cpuidy.h"
#include "common.h"

//!< MemSizeField
#define PFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define DFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define FLEXNVM_SIZE_FIELD          ((SIM->FCFG1 & SIM_FCFG1_NVMSIZE_MASK)>>SIM_FCFG1_NVMSIZE_SHIFT)
#define EEPORM_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_EESIZE_MASK)>>SIM_FCFG1_EESIZE_SHIFT)  
#define RAM_SIZE_FIELD              ((SIM->SOPT1 & SIM_SOPT1_RAMSIZE_MASK)>>SIM_SOPT1_RAMSIZE_SHIFT)            
static const uint32_t CPUIDY_PFlashSizeTable[16] = {8, 16, 24, 32, 48, 64, 96 ,128, 192, 256, 0, 512, 0, 0, 0, 0};
static const uint32_t CPUIDY_RAMSizeTable[16] = {4, 8, 12, 16, 24, 32, 48, 64, 96, 128, 0, 0, 0, 0, 0, 0};
static const uint32_t CPUIDY_EEPORMSizeTable[16] = {16*1024, 8*1024, 4*1024, 2*1024, 1*1024, 512, 256, 128, 64, 32, 0, 0, 0, 0, 0};
static const uint32_t CPUIDY_NVMSizeTable[16] = {8, 16, 24, 32, 48, 64, 96, 128, 192, 256 ,0, 512, 0, 0, 0, 0};
//!< PinCountField
#define PINCOUNT_FIELD              ((SIM->SDID & SIM_SDID_PINID_MASK)>>SIM_SDID_PINID_SHIFT)
static const uint32_t CPUIDY_PinCountTable[16] = {16, 24, 32, 46, 48, 64, 80, 81, 100, 121, 144, 168, 196, 200, 256};

//!< FamilyTypeField
#define FAMID_FIELD                 ((SIM->SDID & SIM_SDID_FAMID_MASK)>>SIM_SDID_FAMID_SHIFT) 
static const char *CPUIDY_FamIDTable[8] = 
{
    "K10",
    "K20",
    "K61 or K30",
    "K40",
    "K60",
    "K70",
    "K50 or K52",
    "K51 or K53",
};




char *CPUIDY_GetFamID(void)
{
    return (char*)CPUIDY_FamIDTable[FAMID_FIELD];
}


State_Type CPUIDY_GetPinCount(uint32_t *pinCount)
{
    *pinCount = 0;
    State_Type ret;
    *pinCount = CPUIDY_PinCountTable[PINCOUNT_FIELD];
    (*pinCount != 0)?(ret = kStatus_Success):(ret = kStatusFail);
		return ret;
}


State_Type CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName, uint32_t *memSizeInKB)
{
    *memSizeInKB = 0;
    State_Type ret;
    if(memSizeName >= kMemNameCount)
		{
        ret = kStatusInvalidArgument;
		}
    switch (memSizeName)
		{
        case kPFlashSizeInKB:
            *memSizeInKB = CPUIDY_PFlashSizeTable[PFLASH_SIZE_FIELD];
				    break;
        case kDFlashSizeInKB:
                *memSizeInKB = 0;
            break;
        case kFlexNVMSizeInKB:
            *memSizeInKB = CPUIDY_NVMSizeTable[FLEXNVM_SIZE_FIELD];
            break;
        case kEEPORMSizeInByte:
            *memSizeInKB = CPUIDY_EEPORMSizeTable[EEPORM_SIZE_FIELD];
            break;
        case kRAMSizeInKB:
            *memSizeInKB = CPUIDY_RAMSizeTable[RAM_SIZE_FIELD];
            break;
				default:
            break;
		}
    (*memSizeInKB != 0)?(ret = kStatus_Success):(ret = kStatusFail);
		return ret;
}





