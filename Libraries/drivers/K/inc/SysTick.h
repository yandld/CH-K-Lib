#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "common.h"

//API functions
void SYSTICK_DelayInit(void);
void SYSTICK_Init(uint32_t timeInUs);
void SYSTICK_Cmd(FunctionalState NewState);
void SYSTICK_ITConfig(FunctionalState NewState);
void SYSTICK_DelayUs(uint32_t us);
void SYSTICK_DelayMs(uint32_t ms);

#endif

