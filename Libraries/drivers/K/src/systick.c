/**
  ******************************************************************************
  * @file    systick.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "systick.h"
#include "clock.h"

static uint32_t fac_us = 0;     //!< usDelay Mut
static uint32_t fac_ms = 0;


//! @defgroup CHKinetis
//! @{


//! @defgroup SysTick
//! @brief SysTick API functions
//! @{

 /**
 * @brief  初始化SysTick时钟
 * 
 * @code
 *      // 初始化SysTick时钟 设定中断周期为10000us(10ms)
 *      SYSTICK_Init(10000);
 * @endcode
 * @param  timeInUs: 中断周期 单位us
 * @retval None
 */
void SYSTICK_Init(uint32_t timeInUs)
{
    // Set clock source = core clock
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    CLOCK_GetClockFrequency(kCoreClock, &fac_us);
    fac_us /= 1000000;
    fac_ms = fac_us * 1000;
    //every 100ms in a int
    SysTick->LOAD = fac_us * timeInUs;
}

 /**
 * @brief  初始化SysTick为延时应用 初始化后 就可以调用 DelayMs DelayUs
 * 
 * @code
 *      //将SysTick用作延时的初始化 初始化后 系统延时20ms
 *      SYSTICK_DelayInit();
 *      SYSTICK_DelayMs(20);
 * @endcode
 * @param  None
 * @retval None
 */
void SYSTICK_DelayInit(void)
{
    SYSTICK_Init(1234);
    SYSTICK_Cmd(ENABLE);
    SYSTICK_ITConfig(DISABLE);
}

 /**
 * @brief  开启或者终止SysTick时钟
 * 
 * @code
 *      //初始化并开启时钟
 *      SYSTICK_Init(10000);
 *      SYSTICK_Cmd(ENABLE);
 * @endcode
 * @param  NewState:使能或者关闭
 *         @arg ENABLE:使能
 *         @arg DISABLE:禁止
 * @retval None
 */
void SYSTICK_Cmd(FunctionalState NewState)
{
    (ENABLE == NewState)?(SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk):(SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk);
}

 /**
 * @brief  开启SysTick中断
 * 
 * @code
 *      //初始化并开启时钟
 *      SYSTICK_Init(10000);
 *      SYSTICK_Cmd(ENABLE);
 *      SYSTICK_ITConfig(ENABLE);
 * @endcode
 * @param  NewState:使能或者关闭
 *         @arg ENABLE:使能
 *         @arg DISABLE:禁止
 * @retval None
 */
void SYSTICK_ITConfig(FunctionalState NewState)
{
    (ENABLE == NewState)?(SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk):(SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk);
}

 /**
 * @brief  SYSTICK延时
 * 
 * @code
 *      //延时100us
 *      SYSTICK_DelayUs(100);
 * @endcode
 * @param  us:延时
 * @retval None
 */
void SYSTICK_DelayUs(uint32_t us)
{
    uint32_t temp;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
}

 /**
 * @brief  SYSTICK延时
 * 
 * @code
 *      //延时100ms
 *      SYSTICK_DelayMs(100);
 * @endcode
 * @param  us:延时
 * @retval None
 */
void SYSTICK_DelayMs(uint32_t ms)
{
    uint32_t temp;
    uint32_t i;
    SysTick->LOAD = fac_ms;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    for(i = 0; i < ms; i++)
	{
		SysTick->VAL = 0;
		do
		{
			temp = SysTick->CTRL;
		}
        while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
	}
}
 
 
//! @}

//! @}

