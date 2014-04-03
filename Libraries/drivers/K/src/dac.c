/**
  ******************************************************************************
  * @file    dac.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
 #include "dac.h"
 #include "common.h"
 
 /* global vars */
static DAC_Type * const DAC_InstanceTable[] = DAC_BASES;
static DAC_CallBackType DAC_CallBackTable[ARRAY_SIZE(DAC_InstanceTable)] = {NULL};
static const IRQn_Type DAC_IRQnTable[] = 
{
    DAC0_IRQn,
    DAC1_IRQn,
};


void DAC_Init(DAC_InitTypeDef* DAC_InitStruct)
{
    SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
    /* trigger select */
    switch(DAC_InitStruct->triggerMode)
    {
        case kDAC_TriggerSoftware:
            DAC_InstanceTable[DAC_InitStruct->instance]->C0 |= DAC_C0_DACTRGSEL_MASK;
            break;
        case kDAC_TriggerHardware:
            DAC_InstanceTable[DAC_InitStruct->instance]->C0 &= ~DAC_C0_DACTRGSEL_MASK;
            break;
        default:
            break;
    }
    /* buffer mode */
    switch(DAC_InitStruct->bufferMode)
    {
        case kDAC_Buffer_Disable:
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 &= ~DAC_C1_DACBFEN_MASK;
            break;
        case kDAC_Buffer_Normal:
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 |= DAC_C1_DACBFEN_MASK;
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 &= ~DAC_C1_DACBFMD_MASK;
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 |= DAC_C1_DACBFMD(0);
            break;
        case kDAC_Buffer_Swing:
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 |= DAC_C1_DACBFEN_MASK;
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 &= ~DAC_C1_DACBFMD_MASK;
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 |= DAC_C1_DACBFMD(1);
            break;
        case kDAC_Buffer_OneTimeScan:
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 |= DAC_C1_DACBFEN_MASK;
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 &= ~DAC_C1_DACBFMD_MASK;
            DAC_InstanceTable[DAC_InitStruct->instance]->C1 |= DAC_C1_DACBFMD(2);
            break;
        default:
            break;
    }
    /* reference */
    switch(DAC_InitStruct->referenceMode)
    {
        case kDAC_Reference_1:
            DAC_InstanceTable[DAC_InitStruct->instance]->C0 &= ~DAC_C0_DACRFS_MASK;
            break;
        case kDAC_Reference_2:
            DAC_InstanceTable[DAC_InitStruct->instance]->C0 |= DAC_C0_DACRFS_MASK;
            break;
        default:
            break;
    }
    /* clear flags */
    DAC_InstanceTable[DAC_InitStruct->instance]->SR &= ~DAC_SR_DACBFRPBF_MASK;
    DAC_InstanceTable[DAC_InitStruct->instance]->SR &= ~DAC_SR_DACBFRPTF_MASK;
    DAC_InstanceTable[DAC_InitStruct->instance]->SR &= ~DAC_SR_DACBFWMF_MASK;
    /* enable module */
    DAC_InstanceTable[DAC_InitStruct->instance]->C0 |= DAC_C0_DACEN_MASK;
}
 
uint32_t DAC_GetBufferReadPointer(uint32_t instance)
{
    return (DAC_InstanceTable[instance]->C2 & DAC_C2_DACBFRP_MASK) >> DAC_C2_DACBFRP_SHIFT;
}

void DAC_SetBufferReadPointer(uint32_t instance, uint32_t value)
{
    DAC_InstanceTable[instance]->C2 &= ~DAC_C2_DACBFRP_MASK;
    DAC_InstanceTable[instance]->C2 |= DAC_C2_DACBFRP(value);
}

/* max 15 */
void DAC_SetBufferUpperLimit(uint32_t instance, uint32_t value)
{
    DAC_InstanceTable[instance]->C2 &= ~DAC_C2_DACBFUP_MASK;
    DAC_InstanceTable[instance]->C2 |= DAC_C2_DACBFUP(value);
}

void DAC_ITDMAConfig(uint32_t instance, DAC_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kDAC_IT_Disable:
            NVIC_DisableIRQ(DAC_IRQnTable[instance]);
            break;
        case kDAC_DMA_Disable:
            DAC_InstanceTable[instance]->C1 &= ~DAC_C1_DMAEN_MASK;
            break;
        case kDAC_IT_BufferPointer_WaterMark:
            DAC_InstanceTable[instance]->C0 |= DAC_C0_DACBWIEN_MASK;
            NVIC_EnableIRQ(DAC_IRQnTable[instance]);
            break;
        case kDAC_IT_BufferPointer_TopFlag:
            DAC_InstanceTable[instance]->C0 |= DAC_C0_DACBTIEN_MASK;
            NVIC_EnableIRQ(DAC_IRQnTable[instance]);
            break;
        case kDAC_IT_BufferPointer_BottomFlag:
            DAC_InstanceTable[instance]->C0 |= DAC_C0_DACBBIEN_MASK;
            NVIC_EnableIRQ(DAC_IRQnTable[instance]);
            break;
        case kDAC_DMA_BufferPointer_WaterMark:
            DAC_InstanceTable[instance]->C0 |= DAC_C0_DACBWIEN_MASK;
            DAC_InstanceTable[instance]->C1 |= DAC_C1_DMAEN_MASK;
            break;
        case kDAC_DMA_BufferPointer_TopFlag:
            DAC_InstanceTable[instance]->C0 |= DAC_C0_DACBTIEN_MASK;
            DAC_InstanceTable[instance]->C1 |= DAC_C1_DMAEN_MASK;
            break;
        case kDAC_DMA_BufferPointer_BottomFlag:
            DAC_InstanceTable[instance]->C0 |= DAC_C0_DACBBIEN_MASK;
            DAC_InstanceTable[instance]->C1 |= DAC_C1_DMAEN_MASK;
            break;
        default:
            break;
    }
}

/* will only trigger once and will advence the read pointer by one step*/
void DAC_StartConversion(uint32_t instance)
{
    DAC_InstanceTable[instance]->C0 |= DAC_C0_DACSWTRG_MASK;
}

void DAC_SetWaterMark(uint32_t instance, DAC_WaterMarkMode_Type value)
{
	switch(value)
	{
		case kDAC_WaterMark_1Word:
			DAC_InstanceTable[instance]->C1 |= DAC_C1_DACBFWM(0);
			break;
		case kDAC_WaterMark_2Word:
			DAC_InstanceTable[instance]->C1 |= DAC_C1_DACBFWM(1);
			break;
		case kDAC_WaterMark_3Word:
			DAC_InstanceTable[instance]->C1 |= DAC_C1_DACBFWM(2);
			break;
		case kDAC_WaterMark_4Word:
			DAC_InstanceTable[instance]->C1 |= DAC_C1_DACBFWM(3);
			break;
		default:break;
	}
}

void DAC_SetBufferValue(uint32_t instance, uint16_t* buf, uint8_t len)
{
	uint8_t i;
	for(i = 0; i < len; i++)
	{
        DAC_InstanceTable[instance]->DAT[i].DATL = (buf[i] & 0x00FF);
        DAC_InstanceTable[instance]->DAT[i].DATH = (buf[i] & 0xFF00)>>8;
	}                       
}

void DAC_CallbackInstall(uint8_t instance, DAC_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        DAC_CallBackTable[instance] = AppCBFun;
    }
}


void DAC0_IRQHandler(void)
{
    if(DAC_CallBackTable[HW_DAC0] != NULL)
    {
        DAC_CallBackTable[HW_DAC0]();
    }  
}


void DAC1_IRQHandler(void)
{
    if(DAC_CallBackTable[HW_DAC1] != NULL)
    {
        DAC_CallBackTable[HW_DAC1]();
    }  
}
 
 
