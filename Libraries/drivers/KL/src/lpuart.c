#include "lpuart.h"
#include "common.h"



//!< Gloabl Const Table Defination
static IRQn_Type   const LPUART_IRQBase = UART0_IRQn;
static UART0_Type * const LPUART_InstanceTable[] = UART0_BASES;
//static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};

static const uint32_t SIM_LPUARTClockGateTable[] =
{
    SIM_SCGC4_UART0_MASK,
};


void LPUART_Init(LPUART_InitTypeDef * LPUART_InitStruct)
{
    // open clock gate
    SIM->SCGC4 |= SIM_LPUARTClockGateTable[LPUART_InitStruct->instance];
    //disable Tx Rx first
    LPUART_InstanceTable[LPUART_InitStruct->instance]->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
	
}




