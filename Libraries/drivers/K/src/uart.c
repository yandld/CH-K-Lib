/**
  ******************************************************************************
  * @file    uart.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.25
  * @brief   超核K60固件库 UART 串口 驱动库
  ******************************************************************************
  */
#include "uart.h"
#include "gpio.h"
#include "common.h"
#include "clock.h"
#include "stdarg.h"
#include <stdio.h>





//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup UART
//! @brief UART driver modules
//! @{


//! @defgroup UART_Exported_Functions
//! @{
#if (!defined(GPIO_BASES))

    #if     (defined(MK60DZ10))
    #define UART_BASES {UART0, UART1, UART2, UART3, UART4}
    #elif   (defined(MK10D5))
    #define UART_BASES {UART0, UART1, UART2}
    #endif

#endif
//!< Gloabl Const Table Defination
#define UART0_RX_TX_IRQn_OFFSET 2
static IRQn_Type   const UART_IRQRxTxBase = UART0_RX_TX_IRQn;
UART_Type * const UART_InstanceTable[] = UART_BASES;
static UART_CallBackType UART_CallBackTable[ARRAY_SIZE(UART_InstanceTable)] = {NULL};
static uint8_t UART_DebugInstance;
#if (defined(MK60DZ10))
static const RegisterManipulation_Type SIM_UARTClockGateTable[] =
{
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART0_MASK},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART1_MASK},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART2_MASK},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART3_MASK},
    {(void*)&(SIM->SCGC1), SIM_SCGC1_UART4_MASK},
};
#elif (defined(MK10D5))
static const RegisterManipulation_Type SIM_UARTClockGateTable[] =
{
    {(uint32_t)&(SIM->SCGC4), SIM_SCGC4_UART0_MASK},
    {(uint32_t)&(SIM->SCGC4), SIM_SCGC4_UART1_MASK},
    {(uint32_t)&(SIM->SCGC4), SIM_SCGC4_UART2_MASK},
};
#endif



void UART_Init(UART_InitTypeDef* UART_InitStruct)
{
    uint16_t sbr;
    uint8_t brfa; 
    uint32_t clock;
    // enable clock gate
    size_t * SIM_SCGx = (void*) SIM_UARTClockGateTable[UART_InitStruct->instance].register_addr;
    *SIM_SCGx |= SIM_UARTClockGateTable[UART_InitStruct->instance].mask;
    //disable Tx Rx first
    UART_InstanceTable[UART_InitStruct->instance]->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    //get clock
    CLOCK_GetClockFrequency(kBusClock, &clock);
    if((UART_InitStruct->instance == 0) || (UART_InitStruct->instance == 1))
    {
        CLOCK_GetClockFrequency(kCoreClock, &clock); //UART0 UART1使用CoreClock
    }
    sbr = (uint16_t)((clock)/((UART_InitStruct->baudrate)*16));
    brfa = ((clock*2)/(UART_InitStruct->baudrate)-(sbr*32));
    // config baudrate
    UART_InstanceTable[UART_InitStruct->instance]->BDH |= ((sbr>>8)&UART_BDH_SBR_MASK);
    UART_InstanceTable[UART_InitStruct->instance]->BDL = (sbr&UART_BDL_SBR_MASK);
    UART_InstanceTable[UART_InitStruct->instance]->C4 |= brfa&(UART_BDL_SBR_MASK>>3);
    // functional config
    UART_InstanceTable[UART_InitStruct->instance]->C1 &= ~UART_C1_M_MASK; // 8bit
    UART_InstanceTable[UART_InitStruct->instance]->C1 &= ~UART_C1_PE_MASK;// no parity check
    UART_InstanceTable[UART_InitStruct->instance]->S2 &= ~UART_S2_MSBF_MASK; //LSB
    // enable Tx Rx
    UART_InstanceTable[UART_InitStruct->instance]->C2 |= ((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    //link debug instance
    UART_DebugInstance = UART_InitStruct->instance;
}




/**
 * @brief  UART send a byte(blocking function)
 * @param  instance: GPIO instance
 *         @arg HW_UART0
 * @param  ch: byte to send
 * @retval None
 */
void UART_WriteByte(uint8_t instance, uint8_t ch)
{
    while(!(UART_InstanceTable[instance]->S1 & UART_S1_TDRE_MASK));
    UART_InstanceTable[instance]->D = (uint8_t)ch;
}

/**
 * @brief  UART receive a byte(none blocking function)
 * @param  instance: GPIO instance
 *         @arg HW_UART0
 * @param  ch: pointer of byte to received
 * @retval 0:succ 1:fail
 */
uint8_t UART_ReadByte(uint8_t instance, uint8_t *ch)
{
    if((UART_InstanceTable[instance]->S1 & UART_S1_RDRF_MASK) != 0)
    {
        *ch = (uint8_t)(UART_InstanceTable[instance]->D);	
        return 0; 		  
    }
    return 1;
}

void UART_putstr(uint8_t instance, const char *str)
{
    while(*str != '\0')
    {
        UART_WriteByte(instance, *str++);
    }
}

void UART_putnstr(uint8_t instance, const char *str, uint32_t len)
{
    while(len--)
    {
        UART_WriteByte(instance, *str++);
    }
}

void UART_ITDMAConfig(uint8_t instance, UART_ITDMAConfig_Type config, FunctionalState newState)
{
    // disable interrupt and dma first
    NVIC_DisableIRQ((IRQn_Type)(UART_IRQRxTxBase + instance * UART0_RX_TX_IRQn_OFFSET));
    // disable DMA and IT
    switch(config)
    {
        case kUART_ITDMA_Disable:
            break;
        case kUART_IT_TxBTC:
            (ENABLE == newState)?(UART_InstanceTable[instance]->C2 |= UART_C2_TIE_MASK):(UART_InstanceTable[instance]->C2 &= ~UART_C2_TIE_MASK);
            break;
        case kUART_DMA_TxBTC:
            break;
        case kUART_IT_RxBTC:
            (ENABLE == newState)?(UART_InstanceTable[instance]->C2 |= UART_C2_RIE_MASK):(UART_InstanceTable[instance]->C2 &= ~UART_C2_RIE_MASK);
            break;
        case kUART_DMA_RxBTC:
            break;
        default:
            break;
    }
    NVIC_EnableIRQ((IRQn_Type)(UART_IRQRxTxBase + instance * UART0_RX_TX_IRQn_OFFSET));
}


//< this function should be called before ITDMAConfig function
void UART_CallbackInstall(uint8_t instance, UART_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        UART_CallBackTable[instance] = AppCBFun;
    }
}

int UART_printf(const char *format,...)
{
    /*
    int chars;
    int i;
    va_list ap;
    char printbuffer[UART_PRINTF_CMD_LENGTH];
    va_start(ap, format);
    chars = vsnprintf(printbuffer, UART_VSPRINT_MAX_LEN, format, ap);
    va_end(ap);
    for(i = 0; i < chars; i++)
    {
        UART_SendByte(UART_DebugInstance,printbuffer[i]);
    }
    return chars ;
    */
}

/*
static const QuickInit_Type UART_QuickInitTable[] =
{
    { 1, 4, 3, 0, 2, 0}, //UART1_RX_PE01_TX_PE00
    { 0, 5, 4,18, 2, 0}, //UART0_RX_PF17_TX_PF18 4
    { 3, 4, 3, 4, 2, 0}, //UART3_RX_PE05_TX_PE04 3
    { 5, 5, 4,19, 2, 0}, //UART5_RX_PF19_TX_PF20 4
    { 5, 4, 3, 8, 2, 0}, //UART5_RX_PE09_TX_PE08 3
    { 2, 4, 3,16, 2, 0}, //UART2_RX_PE17_TX_PE16 3
    { 4, 4, 3,24, 2, 0}, //UART4_RX_PE25_TX_PE24 3
    { 0, 0, 2, 1, 2, 0}, //UART0_RX_PA01_TX_PA02 2
    { 0, 0, 3,14, 2, 0}, //UART0_RX_PA15_TX_PA14 3
    { 3, 1, 3,10, 2, 0}, //UART3_RX_PB10_TX_PB11 3
    { 0, 1, 3,16, 2, 0}, //UART0_RX_PB16_TX_PB17 3
    { 1, 2, 3, 3, 2, 0}, //UART1_RX_PC03_TX_PC04 3
    { 4, 2, 3,14, 2, 0}, //UART4_RX_PC14_TX_PC15 3
    { 3, 2, 3,16, 2, 0}, //UART3_RX_PC16_TX_PC17 3
    { 2, 3, 3, 2, 2, 0}, //UART2_RX_PD02_TX_PD03 3
    { 0, 3, 3, 6, 2, 0}, //UART0_RX_PD06_TX_PD07 3
    { 2, 5, 4,13, 2, 0}, //UART2_RX_PF13_TX_PF14 4
    { 5, 3, 3, 8, 2, 0}, //UART5_RX_PD08_TX_PD09 3
};

void CalConst(const QuickInit_Type * table, uint32_t size)
{
	uint8_t i =0;
	uint32_t value = 0;
	for(i = 0; i < size; i++)
	{
		value = table[i].ip_instance<<0;
		value|= table[i].io_instance<<3;
		value|= table[i].mux<<6;
		value|= table[i].io_base<<9;
		value|= table[i].io_offset<<14;
		value|= table[i].channel<<19;
		UART_printf("(0x%xU)\r\n",value);
	}
}
*/

uint8_t UART_QuickInit(uint32_t UARTxMAP, uint32_t baudrate)
{
    uint8_t i;
    UART_InitTypeDef UART_InitStruct1;
    QuickInit_Type * pUARTxMap = (QuickInit_Type*)&(UARTxMAP);
    UART_InitStruct1.baudrate = baudrate;
    UART_InitStruct1.instance = pUARTxMap->ip_instance;
    UART_Init(&UART_InitStruct1);
    // init pinmux
    for(i = 0; i < pUARTxMap->io_offset; i++)
    {
        PORT_PinMuxConfig(pUARTxMap->io_instance, pUARTxMap->io_base + i, (PORT_PinMux_Type) pUARTxMap->mux); 
    }
    return pUARTxMap->ip_instance;
}


void UART0_RX_TX_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART0]->D = (uint8_t)ch;
   }
   //Rx
   if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_RIE_MASK))
   {
        ch = (uint8_t)UART_InstanceTable[HW_UART0]->D;
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}

void UART1_RX_TX_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART0]->D = (uint8_t)ch;
   }
   //Rx
   if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_RIE_MASK))
   {
        ch = (uint8_t)UART_InstanceTable[HW_UART0]->D;
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}

void UART2_RX_TX_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART0]->D = (uint8_t)ch;
   }
   //Rx
   if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_RIE_MASK))
   {
        ch = (uint8_t)UART_InstanceTable[HW_UART0]->D;
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}

void UART4_RX_TX_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART_InstanceTable[HW_UART4]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART4]->C2 & UART_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART4])
        {
            UART_CallBackTable[HW_UART4](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART4]->D = (uint8_t)ch;
   }
   //Rx
   if((UART_InstanceTable[HW_UART4]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART4]->C2 & UART_C2_RIE_MASK))
   {
        ch = (uint8_t)UART_InstanceTable[HW_UART4]->D;
        if(UART_CallBackTable[HW_UART4])
        {
            UART_CallBackTable[HW_UART4](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}
//! @}

//! @}

//! @}

