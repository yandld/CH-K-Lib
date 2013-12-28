#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "common.h"


//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup SYSTICK
//! @brief SYSTICK driver modules
//! @{




typedef struct
{
    uint32_t SYSTICK_PeriodInUs;                            //!< Tick Period Us
}SYSTICK_InitTypeDef;


//! @defgroup SYSTICK_Exported_Functions
//! @{

void SYSTICK_Init(SYSTICK_InitTypeDef* SYSTICK_InitStruct);
void SYSTICK_Cmd(FunctionalState NewState);
void SYSTICK_ITConfig(FunctionalState NewState);
void SYSTICK_DelayInit(void);
void SYSTICK_DelayUs(uint32_t us);
void SYSTICK_DelayMs(uint32_t ms);

#endif

//! @}

//! @}

//! @}
