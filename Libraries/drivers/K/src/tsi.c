/**
  ******************************************************************************
  * @file    tsi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
 #include "tsi.h"
 #include "common.h"
 #include "gpio.h"
 
static TSI_CallBackType TSI_CallBackTable[1] = {NULL};
 
static const void* TSI_ChlCNTRTalbe[] =
{
    (void*)&TSI0->CNTR1,
    (void*)&TSI0->CNTR1,
    (void*)&TSI0->CNTR3,
    (void*)&TSI0->CNTR3,
    (void*)&TSI0->CNTR5,
    (void*)&TSI0->CNTR5,
    (void*)&TSI0->CNTR7,
    (void*)&TSI0->CNTR7,
    (void*)&TSI0->CNTR9,
    (void*)&TSI0->CNTR9,
    (void*)&TSI0->CNTR11,
    (void*)&TSI0->CNTR11,
    (void*)&TSI0->CNTR13,
    (void*)&TSI0->CNTR13,
    (void*)&TSI0->CNTR15,
    (void*)&TSI0->CNTR15, 
};

uint32_t TSI_GetCounter(uint32_t chl)
{
    uint32_t i;
    if(chl % 0x02)
    {
        i= ((*(uint32_t*)TSI_ChlCNTRTalbe[chl]) >> 16 )&0x0000FFFF;
    }
    else
    {
        i= ((*(uint32_t*)TSI_ChlCNTRTalbe[chl]) >>  0 )&0x0000FFFF; 
    }  
    return i;
}

static void TSI_Calibration(uint32_t chl, uint32_t threshld)
{
    uint32_t cnt;
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;
    TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
	TSI0->GENCS |= TSI_GENCS_SWTS_MASK;
	while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)){};
    cnt = TSI_GetCounter(chl);
    #if DEBUG
        printf("TSI_Calibration cnt:%d\r\n", cnt);
        printf("TSI_Calibration threshld:%d\r\n", threshld);
    #endif
	TSI0->THRESHLD[chl] = TSI_THRESHLD_HTHH(cnt + threshld) | TSI_THRESHLD_LTHH(cnt - threshld);
    TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK;
}

void TSI_Init(TSI_InitTypeDef* TSI_InitStruct)
{
	SIM->SCGC5 |= (SIM_SCGC5_TSI_MASK); 
    /* config general settings */
    TSI0->GENCS |= ((TSI_GENCS_NSCN(10))|(TSI_GENCS_PS(3)));
    TSI0->SCANC |= ((TSI_SCANC_EXTCHRG(3))|(TSI_SCANC_REFCHRG(31))|(TSI_SCANC_DELVOL(7))|(TSI_SCANC_SMOD(0))|(TSI_SCANC_AMPSC(0)));
    /* enable all pens */
    /* FIXME: seems if there is only one TSI pen, TSI cannot work in continues module(STM=1) */	
    TSI0->PEN |= (1<<TSI_InitStruct->chl|(1<<0));			
    TSI_Calibration(TSI_InitStruct->chl, TSI_InitStruct->threshld);
    /* config trigger mode */
    switch(TSI_InitStruct->triggerMode)
    {
        case kTSI_TriggerSoftware:
            TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
            break;
        case kTSI_TriggerPeriodicalScan:
            TSI0->GENCS |= TSI_GENCS_STM_MASK;
            break;
        default:
            break;
    }
    /* clear all IT flags */
    TSI0->GENCS |= TSI_GENCS_OUTRGF_MASK;	  
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK;	 
	TSI0->GENCS |= TSI_GENCS_EXTERF_MASK;
	TSI0->GENCS |= TSI_GENCS_OVRF_MASK;
	TSI0->STATUS = 0xFFFFFFFF;
    /* enable moudle */
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;
}

uint32_t TSI_QuickInit(uint32_t UARTxMAP)
{
    uint32_t i;
    TSI_InitTypeDef TSI_InitStruct;
    QuickInit_Type * pq = (QuickInit_Type*)&UARTxMAP;
    /* config pinmux */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux);
    }
    /* config TSI moudle */
    TSI_InitStruct.chl = pq->channel;
    TSI_InitStruct.triggerMode = kTSI_TriggerPeriodicalScan;
    TSI_InitStruct.threshld = 200;
    TSI_Init(&TSI_InitStruct);
    return pq->ip_instance;
}


void TSI_ITDMAConfig(TSI_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kTSI_IT_Disable:
            TSI0->GENCS &= ~TSI_GENCS_TSIIE_MASK;
            break;
        case kTSI_IT_OutOfRange:
            TSI0->GENCS |= TSI_GENCS_TSIIE_MASK;
            TSI0->GENCS &= ~TSI_GENCS_ESOR_MASK;
            NVIC_EnableIRQ(TSI0_IRQn);
            break;
        case kTSI_IT_EndOfScan:
            TSI0->GENCS |= TSI_GENCS_TSIIE_MASK;
            TSI0->GENCS |= TSI_GENCS_ESOR_MASK;
            NVIC_EnableIRQ(TSI0_IRQn);
            break; 
        default:
            break;
    }
}

void TSI_CallbackInstall(TSI_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        TSI_CallBackTable[0] = AppCBFun;
    }
}

void TSI0_IRQHandler(void)
{
    uint32_t chl_array;
    /* clear all IT pending bit */
    TSI0->GENCS |= TSI_GENCS_OUTRGF_MASK;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK;
    chl_array = TSI0->STATUS & 0xFFFF;
    TSI0->STATUS = 0xFFFFFFFF;
    if(TSI_CallBackTable[0])
    {
        TSI_CallBackTable[0](chl_array);
    }  
}


/*
 static  QuickInit_Type TSI_QuickInitTable[] =
{
    { 0, 1, 0, 0, 1, 0},    //TSI0_CH0_PB00
    { 0, 0, 0, 0, 1, 1},    //TSI0_CH1_PA00
    { 0, 0, 0, 1, 1, 2},    //TSI0_CH2_PA01
    { 0, 0, 0, 2, 1, 3},    //TSI0_CH3_PA02
    { 0, 0, 0, 3, 1, 4},    //TSI0_CH4_PA03
    { 0, 0, 0, 4, 1, 5},    //TSI0_CH5_PA04
    { 0, 1, 0, 1, 1, 6},    //TSI0_CH6_PB01
    { 0, 1, 0, 2, 1, 7},    //TSI0_CH7_PB02
    { 0, 1, 0, 3, 1, 8},    //TSI0_CH8_PB03
    { 0, 1, 0,16, 1, 9},    //TSI0_CH9_PB16
    { 0, 1, 0,17, 1,10},    //TSI0_CH10_PB17
    { 0, 1, 0,18, 1,11},    //TSI0_CH11_PB18
    { 0, 1, 0,19, 1,12},    //TSI0_CH12_PB19
    { 0, 2, 0, 0, 1,13},    //TSI0_CH13_PC00
    { 0, 2, 0, 1, 1,14},    //TSI0_CH14_PC01
    { 0, 2, 0, 2, 1,15},    //TSI0_CH15_PC02
};
*/

