/**
  ******************************************************************************
  * @file    pit.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __PIT_H__
#define __PIT_H__

#include "common.h"


#ifdef __cplusplus
 extern "C" {
#endif


//!< hardware instances
#define HW_PIT0_CH0   (0x00U)
#define HW_PIT0_CH1   (0x01U)
#define HW_PIT0_CH2   (0x02U)
#define HW_PIT0_CH3   (0x03U)

//!< UART CallBack Type
typedef void (*PIT_CallBackType)(void);

//PIT 初始化结构
typedef struct
{
    uint8_t   instance;       //!< PIT instance
    uint8_t   chl;            //!< channel
    uint32_t  timeInUs;       //!< Tick Period Us
}PIT_InitTypeDef;

//!< API functions
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct);
void PIT_Cmd(FunctionalState NewState);
void PIT_QuickInit(uint8_t chl, uint32_t timeInUs);
void PIT_StartCounting(uint8_t chl);
void PIT_StopCounting(uint8_t chl);
void PIT_CallbackInstall(uint8_t chl, PIT_CallBackType AppCBFun);
void PIT_ITDMAConfig(uint8_t chl, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif

