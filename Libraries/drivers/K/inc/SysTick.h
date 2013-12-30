#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "common.h"


//! @defgroup CHKinetis-K
//! @{

//! @defgroup SYSTICK
//! @brief SYSTICK driver modules
//! @{


typedef enum
{
    kSYSTICK_RawValue,
    kSYSTICK_Us,
    kSYSTICK_Ms,
}SYSTICK_Mode_Type;

//! @defgroup SYSTICK_Exported_Functions
//! @{

void SYSTICK_QuickInit(SYSTICK_Mode_Type mode, uint32_t value);
void SYSTICK_Cmd(FunctionalState NewState);
void SYSTICK_ITConfig(FunctionalState NewState);
void SYSTICK_DelayInit();
void SYSTICK_DelayUs(uint32_t us);
void SYSTICK_DelayMs(uint32_t ms);
uint32_t SYSTICK_ReadCounter(void);
uint32_t SYSTICK_ReadLoadCounter(void);
void SYSTICK_WriteCounter(uint32_t value);
uint32_t SYSTICK_WriteLoadCounter(uint32_t value);

#endif

//! @}

//! @}

//! @}
