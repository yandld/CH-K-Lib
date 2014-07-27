/**
  ******************************************************************************
  * @file    lptmr.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
	
#ifndef __CH_LIB_KL_LPTMR_H__
#define __CH_LIB_KL_LPTMR_H__

#include <stdint.h>

/* LPTMR Callback */
typedef void (*LPTMR_CallBackType)(void);

/* ÖÐ¶Ï¼°DMA */
typedef enum
{
    kLPTMR_IT_TOF,
}LPTMR_ITDMAConfig_Type;



#define LPTMR_ALT1_PA19   () 
#define LPTMR_ALT2_PC05   ()



//!< API functions
//void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct);
//void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct);
//void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config);
//void LPTMR_CallbackInstall(LPTMR_CallBackType AppCBFun);
uint32_t LPTMR_PC_ReadCounter(void);
uint32_t LPTMR_PC_QuickInit(uint32_t MAP);
void LPTMR_ClearCounter(void);






#endif


