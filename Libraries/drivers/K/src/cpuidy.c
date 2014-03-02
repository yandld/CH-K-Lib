#include "cpuidy.h"
#include "common.h"

//!< MemSizeField
#define PFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define DFLASH_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
#define FLEXNVM_SIZE_FIELD          ((SIM->FCFG1 & SIM_FCFG1_NVMSIZE_MASK)>>SIM_FCFG1_NVMSIZE_SHIFT)
#define EEPORM_SIZE_FIELD           ((SIM->FCFG1 & SIM_FCFG1_EESIZE_MASK)>>SIM_FCFG1_EESIZE_SHIFT)  
#define RAM_SIZE_FIELD              ((SIM->SOPT1 & SIM_SOPT1_RAMSIZE_MASK)>>SIM_SOPT1_RAMSIZE_SHIFT)

//!< MemSizeTable
static const uint32_t CPUIDY_PFlashSizeTable[16] = {8, 16, 24, 32, 48, 64, 96 ,128, 192, 256, 0, 512, 0, 0, 0, 0};
static const uint32_t CPUIDY_RAMSizeTable[16] = {4, 8, 12, 16, 24, 32, 48, 64, 96, 128, 0, 0, 0, 0, 0, 0};
static const uint32_t CPUIDY_EEPORMSizeTable[16] = {16*1024, 8*1024, 4*1024, 2*1024, 1*1024, 512, 256, 128, 64, 32, 0, 0, 0, 0, 0};
static const uint32_t CPUIDY_NVMSizeTable[16] = {8, 16, 24, 32, 48, 64, 96, 128, 192, 256 ,0, 512, 0, 0, 0, 0};

//!< PinCountField
#define PINCOUNT_FIELD              ((SIM->SDID & SIM_SDID_PINID_MASK)>>SIM_SDID_PINID_SHIFT)
static const uint32_t CPUIDY_PinCountTable[16] = {16, 24, 32, 46, 48, 64, 80, 81, 100, 121, 144, 168, 196, 200, 256};

//!< FamilyTypeField
#define FAMID_FIELD                 ((SIM->SDID & SIM_SDID_FAMID_MASK)>>SIM_SDID_FAMID_SHIFT) 

//!< Family Type String
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

//! @defgroup CHKinetis
//! @{


//! @defgroup CPUIDY
//! @brief CPUIDY API functions
//! @{

/**
 * @brief  获得 芯片的系列ID
 *
 * @code
 *      // 打印芯片型号
 *      printf("Family Type:%s\r\n", CPUIDY_GetFamID());
 * @endcode
 * @param  None
 * @retval ID 字符串指针
 */
char *CPUIDY_GetFamID(void)
{
    return (char*)CPUIDY_FamIDTable[FAMID_FIELD];
}

/**
 * @brief  获得 芯片的引脚数
 *
 * @code
 *      // 打印芯片引脚数
 *      uint32_t PinCnt;
 *      CPUIDY_GetPinCount(&PinCnt);
 *      printf("Pin Cnt:%d\r\n", PinCnt);
 * @endcode
 * @param  None
 * @retval None
 */
void CPUIDY_GetPinCount(uint32_t *pinCount)
{
    *pinCount = 0;
    *pinCount = CPUIDY_PinCountTable[PINCOUNT_FIELD];
}

/**
 * @brief  获得芯片ROM/RAM 大小
 *
 * @code
 *      // 打印芯片Pflash 数量
 *      uint32_t PFlashSize;
 *      CPUIDY_GetMemSize(kPFlashSizeInKB, &PFlashSize);
 *      printf("PFlash Size:%dKB\r\n", PFlashSize);
 * @endcode
 * @param  memSizeName: 存储器类型
 *         @arg kPFlashSizeInKB
 *         @arg kDFlashSizeInKB  
 *         @arg kFlexNVMSizeInKB
 *         @arg kEEPORMSizeInByte
 *         @arg kRAMSizeInKB
 * @param  memSizeInKB: 存储器大小 单位KB
 * @retval None
 */
void CPUIDY_GetMemSize(CPUIDY_MemSize_Type memSizeName, uint32_t *memSizeInKB)
{
    *memSizeInKB = 0;
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
}

/**
 * @brief  获得芯片UID信息(全球唯一识别码)
 *
 * @code
 *      // 打印芯片UID
 *      uint32_t UID[4];
 *      CPUIDY_GetUID(UID);
 *      for(i = 0; i < 4; i++)
 *      {
 *          printf("UID[i]:0x%d", UID[i]);
 *      }
 * @endcode
 * @param  memSizeName: 存储器类型
 *         @arg kPFlashSizeInKB
 *         @arg kDFlashSizeInKB  
 *         @arg kFlexNVMSizeInKB
 *         @arg kEEPORMSizeInByte
 *         @arg kRAMSizeInKB
 * @param  memSizeInKB: 存储器大小 单位KB
 * @retval None
 */
void CPUIDY_GetUID(uint32_t * UIDArray)
{
    UIDArray[0] = SIM->UIDL;
    UIDArray[1] = SIM->UIDML;
    UIDArray[2] = SIM->UIDMH;
    UIDArray[3] = SIM->UIDH;
}

//! @}

//! @}

