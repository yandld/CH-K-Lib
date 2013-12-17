#include "wdog.h"

void WDOG_Refresh(void)
{
    SIM->SRVCOP = 0x55;
    SIM->SRVCOP = 0xAA;
}

void WDOG_Init(void)
{
    SIM->COPC &= ~SIM_COPC_COPT_MASK;
    SIM->COPC |= SIM_COPC_COPT(1);
    SIM->COPC |= SIM_COPC_COPCLKS_MASK;
}


