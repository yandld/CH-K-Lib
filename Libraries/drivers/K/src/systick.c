#include "systick.h"
#include "sys.h"
#include "clock.h"
static uint32_t  fac_us = 0; //!< usDelay Mut
static uint32_t fac_ms = 0; //!< msDelay Mut

//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup SYSTICK
//! @brief SYSTICK driver modules
//! @{


void SYSTICK_Init(SYSTICK_InitTypeDef* SYSTICK_InitStruct)
{
    // Set ClockSource = busClock
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
	  CLOCK_GetClockFrequency(kCoreClock, &fac_us);
	  fac_us /= 1000000;
    fac_ms = (uint32_t)fac_us*1000;
    SysTick->LOAD = (SYSTICK_InitStruct->SYSTICK_PeriodInUs)*fac_us;
}

void SYSTICK_Cmd(FunctionalState NewState)
{
    (ENABLE == NewState)?(SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk):(SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk);
}


void SYSTICK_ITConfig(FunctionalState NewState)
{
    (ENABLE == NewState)?(SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk):(SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk);
}

void SYSTICK_DelayInit()
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //! <Set ClockSource = busClock
    CLOCK_GetClockFrequency(kCoreClock, &fac_us);
    fac_us /= 1000000;
    fac_ms = (uint32_t)fac_us*1000;
}

void SYSTICK_DelayUs(uint32_t us)
{
    uint32_t temp;
    SysTick->LOAD = us*fac_us; 					
    SysTick->VAL = 0x00;   							
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		do
		{
			temp = SysTick->CTRL;
		}
    while((temp & 0x01) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));	//等待时间到达   
}

void SYSTICK_DelayMs(uint32_t ms)
{
    uint32_t temp;
    uint16_t i;
		SysTick->LOAD = fac_ms; 	
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    for(i = 0; i < ms; i++)
    {
        SysTick->VAL = 0x00;
		    do
		    {
            temp = SysTick->CTRL;
		    }
		    while((temp & 0x01) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));	//等待时间到达   
    }
}

uint32_t SYSTICK_GetCurrentCounter(void)
{
     return (SysTick->VAL);  
}

uint32_t SYSTICK_GetReloadValue(void)
{
     return (SysTick->LOAD);  
}

void SYSTICK_SetCurrentCounter(uint32_t Value)
{
    SysTick->VAL = Value;
}

uint32_t SYSTICK_SetReloadValue(uint32_t Value)
{
    SysTick->LOAD = Value;
    return 0;
}

//! @}

//! @}

