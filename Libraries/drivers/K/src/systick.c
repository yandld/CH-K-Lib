#include "systick.h"
#include "sys.h"
#include "clock.h"
static uint32_t fac_us = 0;    //!< usDelay Mut
static uint32_t fac_ms = 0;     //!< msDelay Mut

//! @defgroup CHKinetis-K
//! @{

//! @defgroup SYSTICK
//! @brief SYSTICK driver modules
//! @{


void SYSTICK_QuickInit(SYSTICK_Mode_Type mode, uint32_t value)
{
    // Set ClockSource = CoreClock
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    CLOCK_GetClockFrequency(kCoreClock, &fac_us);
    fac_us /= 1000000;
    fac_ms = (uint32_t)fac_us*1000;
    switch(mode)
    {
        case kSYSTICK_RawValue:
            if(value > SysTick_LOAD_RELOAD_Msk)
            {
                value = SysTick_LOAD_RELOAD_Msk;
            }
            SysTick->LOAD = value;
            break;
        case kSYSTICK_Us:
            if((value*fac_us) > SysTick_LOAD_RELOAD_Msk)
            {
                value = SysTick_LOAD_RELOAD_Msk/fac_us;
            }
            SysTick->LOAD = fac_us*value;
            break;
        case kSYSTICK_Ms:
            if((value*fac_ms) > SysTick_LOAD_RELOAD_Msk)
            {
                value = SysTick_LOAD_RELOAD_Msk/fac_ms;
            }
            SysTick->LOAD = fac_ms*value;     
            break;
    }
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
    SYSTICK_QuickInit(kSYSTICK_RawValue, SysTick_LOAD_RELOAD_Msk);
    SYSTICK_ITConfig(DISABLE);
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
    uint32_t i;
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

uint32_t SYSTICK_ReadCounter(void)
{
     return (SysTick->VAL);  
}

uint32_t SYSTICK_ReadLoadCounter(void)
{
     return (SysTick->LOAD);  
}

void SYSTICK_WriteCounter(uint32_t value)
{
    SysTick->VAL = value;
}

uint32_t SYSTICK_WriteLoadCounter(uint32_t value)
{
    SysTick->LOAD = value;
    return 0;
}

//! @}

//! @}

