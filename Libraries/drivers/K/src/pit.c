#include "pit.h"

static uint8_t  fac_us = 0; //!< usDelay Mut
static uint32_t fac_ms = 0; //!< msDelay Mut

//! @defgroup CH_Periph_Driver
//! @{
	
//! @defgroup PIT
//! @brief PIT driver modules
//! @{

void PIT_Init(PIT_InitTypeDef* PIT_InitStruct)
{
    // Enable Clock Gate
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    GetCPUInfo();
    fac_us = CPUInfo.m_BusClockInHz/1000000;
    fac_ms = (uint32_t)fac_us*1000;
    // Set Load Value
    PIT->CHANNEL[PIT_InitStruct->PITxMap].LDVAL = (uint32_t)fac_us * PIT_InitStruct->PIT_PeriodInUs;
    // Enable PIT Moude
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	  // Start Timer
    PIT->CHANNEL[PIT_InitStruct->PITxMap].TCTRL |= (PIT_TCTRL_TEN_MASK);
}


void PIT_ITConfig(PIT_MapSelect_TypeDef PITx, FunctionalState NewState)
{
    IRQn_Type  IRQn;
    // Find IRQ Number
    switch((uint32_t)PITx)
    {
        case (uint32_t)PIT0:
            IRQn = PIT0_IRQn;
            break;
        case (uint32_t)PIT1:
            IRQn = PIT1_IRQn;
            break;
        case (uint32_t)PIT2:
            IRQn = PIT2_IRQn;
            break;
        case (uint32_t)PIT3:
            IRQn = PIT3_IRQn;
            break;
        default:
            break;
    }
		// Clear IT Pending Bit First
		if(ENABLE == NewState)
		{
			PIT->CHANNEL[PITx].TFLG |= PIT_TFLG_TIF_MASK;
		}
    (ENABLE == NewState)?(PIT->CHANNEL[PITx].TCTRL |= PIT_TCTRL_TIE_MASK):(PIT->CHANNEL[PITx].TCTRL &= ~PIT_TCTRL_TIE_MASK);
		(ENABLE == NewState)?(NVIC_EnableIRQ(IRQn)):(NVIC_DisableIRQ(IRQn));
}

ITStatus PIT_GetITStates(PIT_MapSelect_TypeDef PITx)
{
    if(PIT->CHANNEL[PITx].TFLG & PIT_TFLG_TIF_MASK)
    {
        return SET;
		}
    else
		{
        return RESET;
		}
}

void PIT_ClearITPendingBit(PIT_MapSelect_TypeDef PITx)
{
    PIT->CHANNEL[PITx].TFLG |= PIT_TFLG_TIF_MASK;
}

void PIT_ClearAllITPendingBit(void)
{
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[2].TFLG |= PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[3].TFLG |= PIT_TFLG_TIF_MASK;
}

void PIT_DelayInit(void)
{
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.PITxMap = PIT_DELAY_CHL;
    PIT_InitStruct1.PIT_PeriodInUs = 1000*1000;
    PIT_Init(&PIT_InitStruct1);
    // Disable IT
    PIT_ITConfig(PIT_DELAY_CHL, DISABLE);
	  PIT_Stop(PIT_DELAY_CHL);
}

uint32_t PIT_GetReloadValue(PIT_MapSelect_TypeDef PITx)
{
    return 	PIT->CHANNEL[PITx].LDVAL;  
}

void PIT_SetReloadValue(PIT_MapSelect_TypeDef PITx, uint32_t Value)
{
    PIT->CHANNEL[PITx].LDVAL = Value;
}

uint32_t PIT_GetCurrentCounter(PIT_MapSelect_TypeDef PITx)
{
    return PIT->CHANNEL[PITx].CVAL;
}

void PIT_Start(PIT_MapSelect_TypeDef PITx)
{
    PIT->CHANNEL[PITx].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

void PIT_Stop(PIT_MapSelect_TypeDef PITx)
{
    PIT->CHANNEL[PITx].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
}


void PIT_Cmd(FunctionalState NewState)
{
    (ENABLE == NewState)?(PIT->MCR &= ~PIT_MCR_MDIS_MASK):(PIT->MCR |= PIT_MCR_MDIS_MASK);
}

void PIT_DelayUs(uint32_t us)
{
    PIT_SetReloadValue(PIT_DELAY_CHL, us*fac_us);
    PIT_Start(PIT_DELAY_CHL);
    PIT_ClearITPendingBit(PIT_DELAY_CHL);
    while(PIT_GetITStates(PIT_DELAY_CHL) == RESET);
	  PIT_Stop(PIT_DELAY_CHL);
}

void PIT_DelayMs(uint32_t ms)
{
    PIT_SetReloadValue(PIT_DELAY_CHL, ms*fac_ms);
    PIT_Start(PIT_DELAY_CHL);
    PIT_ClearITPendingBit(PIT_DELAY_CHL);
    while(PIT_GetITStates(PIT_DELAY_CHL) == RESET);
	  PIT_Stop(PIT_DELAY_CHL);
}



//! @}

//! @}



