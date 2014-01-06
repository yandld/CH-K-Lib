#include "systick.h"
#include "sys.h"
#include "clock.h"
static uint32_t fac_us = 0;     //!< usDelay Mut

static uint32_t life_time_tick;
//! @defgroup CHKinetis-K
//! @{

//! @defgroup SYSTICK
//! @brief SYSTICK driver modules
//! @{

void SYSTICK_Init(void)
{
    // Set ClockSource = CoreClock
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    CLOCK_GetClockFrequency(kCoreClock, &fac_us);
    fac_us /= 1000000;
    SysTick->LOAD = fac_us*1000*100; //every 100ms in a int
    SYSTICK_Cmd(ENABLE);
    SYSTICK_ITConfig(ENABLE);
    life_time_tick = 0;
}




void SYSTICK_Cmd(FunctionalState NewState)
{
    (ENABLE == NewState)?(SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk):(SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk);
}

void SYSTICK_ITConfig(FunctionalState NewState)
{
    (ENABLE == NewState)?(SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk):(SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk);
}

void SYSTICK_DelayUs(uint32_t us)
{
    uint32_t init, curr, us2;
    curr = init = SYSTICK_ReadLifeTimeCounter();
    us2 = us;
    do
    {
        curr = SYSTICK_ReadLifeTimeCounter() - init;
    }while(curr < us);
}

void SYSTICK_DelayMs(uint32_t ms)
{
    uint32_t init, curr, ms2;
    ms2 = ms;
    init = SYSTICK_ReadLifeTimeCounter();
    do
    {
        curr = SYSTICK_ReadLifeTimeCounter() - init;
    }while(curr < ms2);
}

uint32_t SYSTICK_ReadLifeTimeCounter(void)
{
    return (life_time_tick + ((SysTick->LOAD - SysTick->VAL)/fac_us)/1000);
}


void SysTick_Handler(void)
{
    life_time_tick += 100;
}

//! @}

//! @}

