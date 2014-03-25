/**
  ******************************************************************************
  * @file    lptmr.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "lptmr.h"
#include "gpio.h"



static LPTMR_CallBackType LPTMR_CallBackTable[1] = {NULL};
static const IRQn_Type PIT_IRQnTable[] = 
{
    LPTimer_IRQn,
};

void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct)
{
	// open clock gate
	SIM->SCGC5 |= SIM_SCGC5_LPTIMER_MASK; 
    LPTMR0->CSR = 0x00; 
    LPTMR0->PSR = 0x00;
    LPTMR0->CMR = 0x00;
    // disable module first
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    // free counter will reset whenever compare register is writtened.
    LPTMR0->CSR &= ~LPTMR_CSR_TFC_MASK;     
    // timer counter mode
    LPTMR0->CSR &= ~LPTMR_CSR_TMS_MASK; 
	// bypass the prescaler, which mean we use 1KHZ LPO directly
    LPTMR0->PSR = LPTMR_PSR_PCS(1)| LPTMR_PSR_PBYP_MASK; 
    // set CMR(compare register)
    LPTMR0->CMR = LPTMR_CMR_COMPARE(LPTMR_TC_InitStruct->timeInMs);
    // enable moudle
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;  
}


void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct)
{
	// open clock gate
	SIM->SCGC5 |= SIM_SCGC5_LPTIMER_MASK; 
    LPTMR0->CSR = 0x00; 
    LPTMR0->PSR = 0x00;
    LPTMR0->CMR = 0x00;
    // disable module first
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    // free counter will reset whenever compare register is writtened.
    LPTMR0->CSR &= ~LPTMR_CSR_TFC_MASK;     
    // timer counter mode
    LPTMR0->CSR |= LPTMR_CSR_TMS_MASK; 
	// bypass the glitch filter, which mean we use 1KHZ LPO directly
    LPTMR0->PSR = LPTMR_PSR_PCS(1)| LPTMR_PSR_PBYP_MASK; 
    // set CMR(compare register)
    LPTMR0->CMR = LPTMR_CMR_COMPARE(LPTMR_PC_InitStruct->counterOverflowValue);
    // input source
    switch(LPTMR_PC_InitStruct->inputSource)
    {
        case kLPTMR_PC_InputSource_CMP0:
            LPTMR0->CSR |= LPTMR_CSR_TPS(0);
            break;
        case kLPTMR_PC_InputSource_ALT1:
            LPTMR0->CSR |= LPTMR_CSR_TPS(1);
            break;
        case kLPTMR_PC_InputSource_ALT2:
            LPTMR0->CSR |= LPTMR_CSR_TPS(2);
            break; 
        default:
            break;
    }
    // pin polarity
    switch(LPTMR_PC_InitStruct->pinPolarity)
    {
        case kLPTMR_PC_PinPolarity_RigsingEdge:
            LPTMR0->CSR &= ~LPTMR_CSR_TPP_MASK;
            break;
        case kLPTMR_PC_PinPolarity_FallingEdge:
            LPTMR0->CSR |= LPTMR_CSR_TPP_MASK;
            break;
        default:
            break;
    }
    // enable moudle
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; 
}

void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config)
{
    switch (config)
    {
        case kLPTMR_IT_Disable:
            NVIC_DisableIRQ(PIT_IRQnTable[0]);
            LPTMR0->CSR &= ~LPTMR_CSR_TIE_MASK;
            break;
        case kLPTMR_IT_TOF:
            NVIC_EnableIRQ(PIT_IRQnTable[0]);
            LPTMR0->CSR |= LPTMR_CSR_TIE_MASK;
            break;
        default:
            break;
    }
}

void LPTMR_CallbackInstall(LPTMR_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        LPTMR_CallBackTable[0] = AppCBFun;
    }
}

uint32_t LPTMR_PC_ReadCounter(void)
{
	return (uint32_t)((LPTMR0->CNR & LPTMR_CNR_COUNTER_MASK) >> LPTMR_CNR_COUNTER_SHIFT); 
}

uint32_t LPTMR_PC_QuickInit(uint32_t LPTMRxMAP)
{
    uint32_t i;
    QuickInit_Type * pq = (QuickInit_Type*)&(LPTMRxMAP);
    LPTMR_PC_InitTypeDef LPTMR_PC_InitStruct1;
    LPTMR_PC_InitStruct1.counterOverflowValue = 0xFFFF;
    switch(pq->channel)
    {
        case 1:
            LPTMR_PC_InitStruct1.inputSource = kLPTMR_PC_InputSource_ALT1;
            break;
        case 2:
            LPTMR_PC_InitStruct1.inputSource = kLPTMR_PC_InputSource_ALT2;
            break;
        default:
            break;
    }
    LPTMR_PC_InitStruct1.pinPolarity = kLPTMR_PC_PinPolarity_RigsingEdge;
    /* init pinmux */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
    /* init moudle */
    LPTMR_PC_Init(&LPTMR_PC_InitStruct1);
    return pq->ip_instance;
}

void LPTMR_ClearCount(void)
{
    //disable and reenable moudle to clear counter
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}

void LPTimer_IRQHandler(void)
{
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
    if(LPTMR_CallBackTable[0])
    {
        LPTMR_CallBackTable[0]();
    }
}


#if 0
static const QuickInit_Type LPTMR_QuickInitTable[] = 
{
    { 0, 0, 6,19, 1, 1}, //LPTMR_ALT1_PA19 6
    { 0, 2, 4, 5, 1, 2}, //LPTMR_ALT2_PC05 4
};
#endif

