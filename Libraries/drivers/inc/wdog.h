#include "sys.h"

typedef enum
{
    kWDOG_Mode_NORMAL,            //!< WDOG Normal Mode
    kWDOG_Mode_WINDOW,            //!< WDOG Window Mode
}WDOG_ModeSelect_TypeDef;


typedef struct
{
    WDOG_ModeSelect_TypeDef WDOG_Mode;     //!< WDOG Mode Select
    uint32_t WDOG_TimeOutInUs;             //!< WDOG Interval Time In US
    uint32_t WDOG_WindowTimeInUs;          //!< WDOG Windows Time In US
}WDOG_InitTypeDef;


void WDOG_QuickInit(uint32_t TimeInUs);
void WDOG_Init(WDOG_InitTypeDef* WDOG_InitStruct);
void WDOG_Cmd(FunctionalState NewState);
void WDOG_ITConfig(FunctionalState NewState);
void WDOG_ClearITPendingBit(void);
void WDOG_ClearAllITPendingBit(void);
uint32_t WDOG_ReadResetCounter(void);
void WDOG_ClearResetCounter(void);
void WDOG_Refresh(void);


