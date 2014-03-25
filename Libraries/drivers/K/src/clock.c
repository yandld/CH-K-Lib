/**
  ******************************************************************************
  * @file    clock.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
	* @note    此文件为内部文件，用于设置和获取芯片时钟频率，用户无需调用和修改  
  ******************************************************************************
  */
#include "clock.h"
#include "common.h"

#define MCGOUT_TO_CORE_DIVIDER           (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT) + 1)
#define MCGOUT_TO_SYSTEM_DIVIDER         (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT) + 1)
#define MCGOUT_TO_BUS_DIVIDER            (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT) + 1)
#define MCGOUT_TO_PERIPHERAL_DIVIDER     (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT) + 1)
#define MCGOUT_TO_FLEXBUS_DIVIDER        (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK)>>SIM_CLKDIV1_OUTDIV3_SHIFT) + 1)
#define MCGOUT_TO_FLASH_DIVIDER          (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT) + 1)

#define MCGOUT_TO_CORE_DIV_SET(x)        SIM_CLKDIV1_OUTDIV1(x)
#define MCGOUT_TO_SYSTEM_DIV_SET(x)      SIM_CLKDIV1_OUTDIV1(x)
#define MCGOUT_TO_BUS_DIV_SET(x)         SIM_CLKDIV1_OUTDIV2(x)
#define MCGOUT_TO_PERIPHERAL_DIV_SET(x)  SIM_CLKDIV1_OUTDIV2(x)
//#define MCGOUT_TO_FLEXBUS_DIV_SET(x)     SIM_CLKDIV1_OUTDIV3(x)
#define MCGOUT_TO_FLASH_DIV_SET(x)       SIM_CLKDIV1_OUTDIV4(x)


 /**
 * @brief  设置各个时钟的分频系数
 * @code
 *      // 设置内核时钟的输出频率为1/3分频
 *      CLOCK_SetClockDivider(kMcgOut2CoreDivider, kClockSimDiv3);
 * @endcode
 * @param  clockDivName: 时钟名称
 *         @arg kMcgOut2CoreDivider   :内核时钟分频
 *         @arg kMcgOut2SystemDivider :系统时钟分频
 *         @arg kMcgOut2BusDivider    :总线时钟分频
 *         @arg kMcgOut2FlashDivider  :flash时钟分频
 * @param  dividerValue: 分频系数
 *         @arg kClockSimDiv2  :2分频
 *         @arg kClockSimDiv3  :3分频
 *         @arg             .  : .      
 *         @arg             .  : .  
 *         @arg             .  : .   
 *         @arg kClockSimDiv16 :16分频
 * @retval None
 */
void CLOCK_SetClockDivider(CLOCK_DividerSource_Type clockDivName, CLOCK_DivideValue_Type dividerValue)
{
    switch (clockDivName)
		{
			case kMcgOut2CoreDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_CORE_DIV_SET(dividerValue);
				break;
			case kMcgOut2SystemDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_SYSTEM_DIV_SET(dividerValue);
				break;
			case kMcgOut2BusDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_BUS_DIV_SET(dividerValue);
				break;
			case kMcgOut2FlashDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_FLASH_DIV_SET(dividerValue);
				break;
			default :
				break;
		}
}

 /**
 * @brief  获得系统各个总线时钟的频率
 * @code
 *         //获得总线时钟频率
 *         uint32_t BusClock;
 *         CLOCK_GetClockFrequency(kBusClock, &BusClock);
 *         //将总线时钟频率显示出来
 *         printf("BusClock:%dHz\r\n", BusClock);
 * @endcode
 * @param  clockName:时钟名称
 *         @arg kCoreClock    :内核时钟
 *         @arg kSystemClock  :系统时钟 = 内核时钟
 *         @arg kBusClock     :总线时钟
 *         @arg kFlexBusClock :Flexbus总线时钟
 *         @arg kFlashClock   :Flash总线时钟
 * @param  FrequenctInHz: 获得频率数据的指针 单位Hz
 * @retval 0: 成功 非0: 错误
 */
int32_t CLOCK_GetClockFrequency(CLOCK_Source_Type clockName, uint32_t* FrequenctInHz)
{
    uint32_t MCGOutClock = 0;
    /* calualte MCGOutClock system_MKxxx.c must not modified*/
    MCGOutClock = SystemCoreClock * MCGOUT_TO_CORE_DIVIDER;
    switch (clockName)
    {
        case kCoreClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_CORE_DIVIDER;
            break;
        case kSystemClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_CORE_DIVIDER;
            break;	
        case kBusClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_BUS_DIVIDER;
            break;
        case kFlashClock:
            *FrequenctInHz = MCGOutClock / MCGOUT_TO_FLASH_DIVIDER;	
            break;
        default:
            return 1;
    }
    return 0;
}



