#include "wdog.h"

static void WDOG_Unlock(void)
{
    WDOG->UNLOCK = 0xC520u;
    WDOG->UNLOCK = 0xD928u;
}

void WDOG_QuickInit(uint32_t TimeInUs)
{
    WDOG_InitTypeDef WDOG_InitStruct1;
    WDOG_InitStruct1.WDOG_Mode = kWDOG_Mode_NORMAL;
    WDOG_InitStruct1.WDOG_TimeOutInUs = TimeInUs;
    WDOG_InitStruct1.WDOG_WindowTimeInUs = TimeInUs;
	  WDOG_Init(&WDOG_InitStruct1);
}

void WDOG_Init(WDOG_InitTypeDef* WDOG_InitStruct)
{
    uint32_t timeValue = 0;
		GetCPUInfo();
	 // WDOG_Cmd(DISABLE);
    WDOG_Unlock();
	  // Perscale = 8
    WDOG->PRESC = WDOG_PRESC_PRESCVAL(7);
	  // Set Timeout Value
    timeValue = ((CPUInfo.m_BusClockInHz/8)/1000000)*(WDOG_InitStruct->WDOG_TimeOutInUs);
		WDOG_Unlock();
    WDOG->TOVALH = (timeValue & 0xFFFF0000)>>16;
    WDOG->TOVALL = (timeValue & 0x0000FFFF)>>0;
	  // Set Window Time Value :Timeout Must Greater Then WindowTime
	  if(WDOG_InitStruct->WDOG_TimeOutInUs > WDOG_InitStruct->WDOG_WindowTimeInUs)
		{
		    timeValue = timeValue - ((CPUInfo.m_BusClockInHz/8)/1000000)*(WDOG_InitStruct->WDOG_WindowTimeInUs);
        WDOG_Unlock();
		    WDOG->WINH = (timeValue & 0xFFFF0000)>>16;
		    WDOG->WINL = (timeValue & 0x0000FFFF)>>0;
		}
    switch((uint32_t)WDOG_InitStruct->WDOG_Mode)
    {
        case kWDOG_Mode_NORMAL:
					  WDOG_Unlock();
            WDOG->STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;
            break;
        case kWDOG_Mode_WINDOW:
            WDOG_Unlock();
            WDOG->STCTRLH |= (WDOG_STCTRLH_WINEN_MASK|WDOG_STCTRLH_WDOGEN_MASK);
            break;		
        default:
            break;
    }
}


void WDOG_Cmd(FunctionalState NewState)
{
    WDOG_Unlock();
    (ENABLE == NewState)?(WDOG->STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK):(WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK);
}

void WDOG_ITConfig(FunctionalState NewState)
{
    WDOG_Unlock();
    (ENABLE == NewState)?(WDOG->STCTRLH |= WDOG_STCTRLH_IRQRSTEN_MASK):(WDOG->STCTRLH &= ~WDOG_STCTRLH_IRQRSTEN_MASK);
    (ENABLE == NewState)?(NVIC_EnableIRQ(Watchdog_IRQn)):(NVIC_DisableIRQ(Watchdog_IRQn));
}

void WDOG_ClearITPendingBit(void)
{
    WDOG->STCTRLL |= WDOG_STCTRLL_INTFLG_MASK;
}

void WDOG_ClearAllITPendingBit(void)
{
    WDOG->STCTRLL |= WDOG_STCTRLL_INTFLG_MASK;
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
	WDOG->REFRESH = 0xA602u;
	WDOG->REFRESH = 0xB480u;
}

