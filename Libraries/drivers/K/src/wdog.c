/**
  ******************************************************************************
  * @file    wdog.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "wdog.h"
#include "common.h"
#include "clock.h"

static WDOG_CallBackType WDOG_CallBackTable[1] = {NULL};

static void WDOG_Unlock(void)
{
    WDOG->UNLOCK = 0xC520u;
    WDOG->UNLOCK = 0xD928u;
}

void WDOG_QuickInit(uint32_t timeInMs)
{
    WDOG_InitTypeDef WDOG_InitStruct1;
    WDOG_InitStruct1.mode = kWDOG_Mode_Normal;
    WDOG_InitStruct1.timeOutInMs = timeInMs;
    WDOG_InitStruct1.windowInMs = timeInMs/2;
    WDOG_Init(&WDOG_InitStruct1);
}

void WDOG_Init(WDOG_InitTypeDef* WDOG_InitStruct)
{
    uint32_t clock;
    uint32_t time_out;
    CLOCK_GetClockFrequency(kBusClock, &clock);
    uint32_t wdag_value = 0x01D3u;
    switch((uint32_t)WDOG_InitStruct->mode)
    {
        case kWDOG_Mode_Normal:
            wdag_value &= ~WDOG_STCTRLH_WINEN_MASK;
            break;
        case kWDOG_Mode_Window:
            wdag_value |= WDOG_STCTRLH_WINEN_MASK;
            break;		
        default:
            break;
    }
    WDOG_Unlock();
    // Set Timeout Value
    time_out = ((clock/8)/1000)*(WDOG_InitStruct->timeOutInMs);
    WDOG->TOVALH = (time_out & 0xFFFF0000)>>16;
    WDOG->TOVALL = (time_out & 0x0000FFFF)>>0;
    // set window time value :timeout must greater then window time
    time_out = ((clock/8)/1000)*(WDOG_InitStruct->windowInMs);
    WDOG->WINH = (time_out & 0xFFFF0000)>>16;
    WDOG->WINL = (time_out & 0x0000FFFF)>>0;
    WDOG->PRESC = WDOG_PRESC_PRESCVAL(7); // perscale = 8
    // enable wdog
    wdag_value |= WDOG_STCTRLH_WDOGEN_MASK;
    WDOG->STCTRLH = wdag_value;
}

void WDOG_ITDMAConfig(FunctionalState NewState)
{
    WDOG_Unlock();
    (ENABLE == NewState)?(WDOG->STCTRLH |= WDOG_STCTRLH_IRQRSTEN_MASK):(WDOG->STCTRLH &= ~(WDOG_STCTRLH_IRQRSTEN_MASK));
    (ENABLE == NewState)?(NVIC_EnableIRQ(Watchdog_IRQn)):(NVIC_DisableIRQ(Watchdog_IRQn));
}

void WDOG_CallbackInstall(WDOG_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        WDOG_CallBackTable[0] = AppCBFun;
    }
}

uint32_t WDOG_ReadResetCounter(void)
{
    return (WDOG->RSTCNT);
}

void WDOG_ClearResetCounter(void)
{
    WDOG->RSTCNT = WDOG_RSTCNT_RSTCNT_MASK;
}

void WDOG_Refresh(void)
{
    uint32_t i;
	WDOG->REFRESH = 0xA602u;
	WDOG->REFRESH = 0xB480u;
    // a gap of more then 20 bus cycle between 2 refresh sequence
    for(i = 0; i < 20; i++)
    {
        __NOP();
    }
}

//貌似进不去中断
void Watchdog_IRQHandler(void)
{
    WDOG->STCTRLL |= WDOG_STCTRLL_INTFLG_MASK;    
    if(WDOG_CallBackTable[0])
    {
        WDOG_CallBackTable[0]();
    }
}

