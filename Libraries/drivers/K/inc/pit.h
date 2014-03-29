/**
  ******************************************************************************
  * @file    pit.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片PIT模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_PIT_H__
#define __CH_LIB_PIT_H__

#include "common.h"

#ifdef __cplusplus
 extern "C" {
#endif


//!< hardware instances
#define HW_PIT_CH0   (0x00U)
#define HW_PIT_CH1   (0x01U)
#define HW_PIT_CH2   (0x02U)
#define HW_PIT_CH3   (0x03U)

//!< PIT CallBack Type
typedef void (*PIT_CallBackType)(void);

//PIT 初始化结构
typedef struct
{
    uint8_t   chl;            //!< channel
    uint32_t  timeInUs;       //!< Tick Period Us
}PIT_InitTypeDef;

//!< PIT 中断DMA配置
typedef enum
{
    kPIT_IT_Disable,        //!< Disable Interrupt
    kPIT_IT_TOF,            //!< Enable time overflow flag
}PIT_ITDMAConfig_Type;

//!< API functions
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct);
void PIT_Cmd(FunctionalState NewState);
void PIT_QuickInit(uint8_t chl, uint32_t timeInUs);
void PIT_StartCounting(uint8_t chl);
void PIT_StopCounting(uint8_t chl);
void PIT_CallbackInstall(uint8_t chl, PIT_CallBackType AppCBFun);
void PIT_ITDMAConfig(uint8_t chl, PIT_ITDMAConfig_Type config);

#ifdef __cplusplus
}
#endif

#endif

