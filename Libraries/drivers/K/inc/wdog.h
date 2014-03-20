/**
  ******************************************************************************
  * @file    wdog.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "common.h"

typedef enum
{
    kWDOG_Mode_Normal,            //!< WDOG Normal Mode
    kWDOG_Mode_Window,            //!< WDOG Window Mode
}WDOG_Mode_Type;


typedef struct
{
    WDOG_Mode_Type      mode;                   //!< WDOG Mode Select
    uint32_t            timeOutInMs;            //!< WDOG Interval Time In US
    uint32_t            windowInMs;             //!< WDOG Windows Time In US
}WDOG_InitTypeDef;

//!< Callback Type
typedef void (*WDOG_CallBackType)(void);

//!< API functions
void WDOG_QuickInit(uint32_t timeInUs);
void WDOG_Init(WDOG_InitTypeDef* WDOG_InitStruct);
void WDOG_ITDMAConfig(FunctionalState NewState);
void WDOG_CallbackInstall(WDOG_CallBackType AppCBFun);
uint32_t WDOG_ReadResetCounter(void);
void WDOG_ClearResetCounter(void);
void WDOG_Refresh(void);


