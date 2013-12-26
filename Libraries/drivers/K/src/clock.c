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




void CLOCK_SetClockDivider(CLOCK_DividerSource_Type clockDivName, CLOCK_DivideValue_Type dividerValue)
{
    switch (clockDivName)
		{
			case kMcgOut2CoreDivider:
			//	SIM->CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV1_MASK;
				SIM->CLKDIV1 |= MCGOUT_TO_CORE_DIV_SET(dividerValue);
				break;
			case kMcgOut2SystemDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_SYSTEM_DIV_SET(dividerValue);
				break;
			case kMcgOut2BusDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_BUS_DIV_SET(dividerValue);
				break;
			case kMcgOut2FlexBusDivider:
		//		SIM->CLKDIV1 |= MCGOUT_TO_FLEXBUS_DIV_SET(dividerValue);
				break;
			case kMcgOut2FlashDivider:
				SIM->CLKDIV1 |= MCGOUT_TO_FLASH_DIV_SET(dividerValue);
				break;
			default :
				break;
		}
}


void CLOCK_GetClockFrequency(CLOCK_Source_Type clockName, uint32_t* FrequenctInHz)
{
    uint32_t MCGOutClock = 0;
		//calualte MCGOutClock
		MCGOutClock = SystemCoreClock * MCGOUT_TO_CORE_DIVIDER;
    
		switch (clockName)
		{
			case kCoreClock:
				*FrequenctInHz = MCGOutClock / MCGOUT_TO_CORE_DIVIDER;
				break;
			case kSystemClock:
				*FrequenctInHz = MCGOutClock / MCGOUT_TO_SYSTEM_DIVIDER;
				break;	
			case kBusClock:
				*FrequenctInHz = MCGOutClock / MCGOUT_TO_BUS_DIVIDER;	
				break;
			case kFlexBusClock:
		//		*FrequenctInHz = MCGOutClock / MCGOUT_TO_FLEXBUS_DIVIDER;		
				break;
			case kFlashClock:
				*FrequenctInHz = MCGOutClock / MCGOUT_TO_FLASH_DIVIDER;		
				break;
			default:
				*FrequenctInHz = 0;
				break;
		}
}




