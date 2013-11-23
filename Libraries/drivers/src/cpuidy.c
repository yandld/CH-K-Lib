#include "cpuidy.h"
#include "common.h"

#define PFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define DFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define FLEXNVMSIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_NVMSIZE_MASK)>>SIM_FCFG1_NVMSIZE_SHIFT)


/*

    kPFlashSizeInKB,                 
    kDFlashSizeInKB,
    kFlexNVMSizeInKB,
    kEEPORMSizeInKB,
    kRAMSizeInKB,
    kMemNameCount,
*/

State_Type CPUIDY_GetPinCount(uint32_t *pinCount)
{
	
	
	
}


State_Type CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName, uint32_t *memSizeInKB)
{
    if(memSizeName >= kMemNameCount)
		{
        return kStatusInvalidArgument;
		}
    switch (memSizeName)
		{
        case kPFlashSizeInKB:
            switch(PFLASH_SIZE_FIELD)
						{
								case 0x00:
                    *memSizeInKB = 8;
										break;
								case 0x01:
                    *memSizeInKB = 16;		
										break;
								case 0x02:
										break;
								case 0x03:
                    *memSizeInKB = 32;
										break;
								case 0x04:
										break;
								case 0x05:
                    *memSizeInKB = 64;
										break;
								case 0x06:
                    *memSizeInKB = 64;
										break;
								case 0x07:
                    *memSizeInKB = 128;
										break;
								case 0x08:
										break;
								case 0x09:
                    *memSizeInKB = 256;
										break;
								case 0x0A:
										break;
								case 0x0B:
                    *memSizeInKB = 512;
										break;
								case 0x0C:
										break;
								case 0x0D:
										break;
								case 0x0E:
										break;
								case 0x0F:
										break;
								default:
										break;
            }	
        case kDFlashSizeInKB:
            break;
        case kFlexNVMSizeInKB:
            break;
        case kEEPORMSizeInKB:
            break;
        case kRAMSizeInKB:
            break;
				default:
            break;
		}
    
	
}







