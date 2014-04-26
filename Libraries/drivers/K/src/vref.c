/**
  ******************************************************************************
  * @file    vref.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
 #include "vref.h"
 #include "common.h"
 
 
void VREF_Init(VREF_InitTypeDef *VREF_InitStruct)
{
    /* open clock gate */
    SIM->SCGC4 |= SIM_SCGC4_VREF_MASK;
    /* enable moudle */
    VREF->SC = VREF_SC_VREFEN_MASK | VREF_SC_REGEN_MASK | VREF_SC_ICOMPEN_MASK | VREF_SC_MODE_LV(VREF_InitStruct->bufferMode);
    VREF->TRM |= VREF_TRM_CHOPEN_MASK;
}

void VREF_QuickInit(void)
{
    VREF_InitTypeDef  VREF_InitStruct1;
    VREF_InitStruct1.instance = 0;
    VREF_InitStruct1.bufferMode = kVREF_BufferMode_Lv1;
    VREF_Init(&VREF_InitStruct1);
}
 
void VREF_DeInit(void)
{
    VREF->SC = 0;
}

void VREF_SetTrimValue(uint32_t val)
{
    VREF->TRM &= ~VREF_TRM_TRIM_MASK;
    VREF->TRM |= VREF_TRM_TRIM(val);
}
 
 
