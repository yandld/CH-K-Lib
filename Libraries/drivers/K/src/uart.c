/**
  ******************************************************************************
  * @file    uart.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "uart.h"
#include "gpio.h"
#include "common.h"
#include "clock.h"

#ifdef UART_USE_STDIO
#if __ICCARM__
#include <yfuns.h>
#endif
#include <stdio.h>
#endif

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
#if (defined(MK60DZ10) || defined(MK40D10) || defined(MK60D10)|| defined(MK10D10))
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
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART0_MASK},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART1_MASK},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART2_MASK},
};
#endif

#ifdef UART_USE_STDIO
#ifdef __CC_ARM // MDK Support
struct __FILE 
{ 
	int handle;
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef¡¯ d in stdio.h. */ 
FILE __stdout; 
int fputc(int ch,FILE *f)
{
	UART_WriteByte(UART_DebugInstance, ch);
	return ch;
}

int fgetc(FILE *f)
{
    uint8_t ch;
    while(UART_ReadByte(UART_DebugInstance, &ch));
    return ch;
}
#elif __ICCARM__ // IAR Support
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        /* This means that we should flush internal buffers.  Since we*/
        /* don't we just return.  (Remember, "handle" == -1 means that all*/
        /* handles should be flushed.)*/
        return 0;
    }
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != _LLIO_STDOUT) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* Send data.*/
    while (size--)
    {
        sputc(*buffer++);
        ++nChars;
    }
    return nChars;
}

#endif // Comiler Support

#else // DO NOT USE STDIO
static void UART_putstr(uint8_t instance, const char *str)
{
    while(*str != '\0')
    {
        UART_WriteByte(instance, *str++);
    }
}

static void printn(unsigned int n, unsigned int b)
{
    static char *ntab = "0123456789ABCDEF";
    unsigned int a, m;
    if (n / b)
    {
        a = n / b;
        printn(a, b);  
    }
    m = n % b;
    UART_WriteByte(UART_DebugInstance, ntab[m]);
}

int UART_printf(const char *fmt, ...)
{
    char c;
    unsigned int *adx = (unsigned int*)(void*)&fmt + 1;
_loop:
    while((c = *fmt++) != '%')
    {
        if (c == '\0') return 0;
        UART_WriteByte(UART_DebugInstance, c);
    }
    c = *fmt++;
    if (c == 'd' || c == 'l')
    {
        printn(*adx, 10);
    }
    if (c == 'o' || c == 'x')
    {
        printn(*adx, c=='o'? 8:16 );
    }
    if (c == 's')
    {
        UART_putstr(UART_DebugInstance, (char*)*adx);
    }
    adx++;
    goto _loop;
    return 0;
}
#endif // UART_USE_STDIO

//! @defgroup CHKinetis-K
//! @{

//! @defgroup UART-K
//! @brief UART-K driver modules
//! @{

//! @defgroup UART-K_API_Functions
//! @{

/**
 * @brief  UART init: enable UART clock Tx and Rx
 *         this function must be called before using uart
 * @param  UART_InitStruct: init type for uart
 * @retval None
 */
void UART_Init(UART_InitTypeDef* UART_InitStruct)
{
    uint16_t sbr;
    uint8_t brfa; 
    uint32_t clock;
	//param check
    assert_param(IS_UART_ALL_INSTANCE(UART_InitStruct->instance));
    // enable clock gate
    size_t * SIM_SCGx = (void*) SIM_UARTClockGateTable[UART_InitStruct->instance].register_addr;
    *SIM_SCGx |= SIM_UARTClockGateTable[UART_InitStruct->instance].mask;
    //disable Tx Rx first
    UART_InstanceTable[UART_InitStruct->instance]->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    //get clock
    CLOCK_GetClockFrequency(kBusClock, &clock);
    if((UART_InitStruct->instance == 0) || (UART_InitStruct->instance == 1))
    {
        CLOCK_GetClockFrequency(kCoreClock, &clock); //UART0 UART1Ê¹ÓÃCoreClock
    }
    sbr = (uint16_t)((clock)/((UART_InitStruct->baudrate)*16));
    brfa = (32*clock/((UART_InitStruct->baudrate)*16)) - 32*sbr;
    // config baudrate
    UART_InstanceTable[UART_InitStruct->instance]->BDH |= UART_BDH_SBR(sbr>>8); 
    UART_InstanceTable[UART_InitStruct->instance]->BDL = UART_BDL_SBR(sbr); 
    UART_InstanceTable[UART_InitStruct->instance]->C4 |= UART_C4_BRFA(brfa);
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
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg HW_UART1
 *         @arg ...
 * @param  ch: byte to send
 * @retval None
 */
void UART_WriteByte(uint8_t instance, uint8_t ch)
{
	//param check
    assert_param(IS_UART_ALL_INSTANCE(instance));
    while(!(UART_InstanceTable[instance]->S1 & UART_S1_TDRE_MASK));
    UART_InstanceTable[instance]->D = (uint8_t)ch;
}

/**
 * @brief  UART receive a byte(none blocking function)
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg HW_UART1
 *         @arg ...
 * @param  ch: pointer of byte to received
 * @retval 0:succ 1:fail
 */
uint8_t UART_ReadByte(uint8_t instance, uint8_t *ch)
{
	//param check
    assert_param(IS_UART_ALL_INSTANCE(instance));
    if((UART_InstanceTable[instance]->S1 & UART_S1_RDRF_MASK) != 0)
    {
        *ch = (uint8_t)(UART_InstanceTable[instance]->D);	
        return 0; 		  
    }
    return 1;
}

/**
 * @brief  UART DMA and Interrupt config
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg HW_UART1
 *         @arg ...
 * @param  config: config, see UART_ITDMAConfig_Type for more details
 * @param  newState:
 *         @arg ENABLE
 *         @arg DISABLE
 * @param  ch: pointer of byte to received
 * @retval None
 */
void UART_ITDMAConfig(uint8_t instance, UART_ITDMAConfig_Type config, FunctionalState newState)
{
    switch(config)
    {
        case kUART_ITDMA_Disable:
            NVIC_DisableIRQ((IRQn_Type)(UART_IRQRxTxBase + instance * UART0_RX_TX_IRQn_OFFSET));
            UART_InstanceTable[instance]->C2 &= ~UART_C2_TIE_MASK;
            UART_InstanceTable[instance]->C2 &= ~UART_C2_RIE_MASK;
            break;
        case kUART_IT_TxBTC:
            (ENABLE == newState)?(UART_InstanceTable[instance]->C2 |= UART_C2_TIE_MASK):(UART_InstanceTable[instance]->C2 &= ~UART_C2_TIE_MASK);
            break;
        case kUART_DMA_TxBTC:
            (ENABLE == newState)?(UART_InstanceTable[instance]->C5 |= UART_C5_TDMAS_MASK):(UART_InstanceTable[instance]->C5 &= ~UART_C5_TDMAS_MASK);
            break;
        case kUART_IT_RxBTC:
            (ENABLE == newState)?(UART_InstanceTable[instance]->C2 |= UART_C2_RIE_MASK):(UART_InstanceTable[instance]->C2 &= ~UART_C2_RIE_MASK);
            break;
        case kUART_DMA_RxBTC:
            (ENABLE == newState)?(UART_InstanceTable[instance]->C5 |= UART_C5_RDMAS_MASK):(UART_InstanceTable[instance]->C5 &= ~UART_C5_RDMAS_MASK);
            break;
        default:
            break;
    }
    NVIC_EnableIRQ((IRQn_Type)(UART_IRQRxTxBase + instance * UART0_RX_TX_IRQn_OFFSET));
}

/**
 * @brief  Install UART Tx Rx Callback function
 *         this function should be called before ITDMAConfig function
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg HW_UART1
 *         @arg ...
 * @param  AppCBFun: pointer of callback function
 * @retval 0:succ 1:fail
 */
void UART_CallbackInstall(uint8_t instance, UART_CallBackType AppCBFun)
{
	//param check
    assert_param(IS_UART_ALL_INSTANCE(instance));
    if(AppCBFun != NULL)
    {
        UART_CallBackTable[instance] = AppCBFun;
    }
}

 /**
 * @brief  quick init for user, do not need init struct. 
 * @code
 *      // Init UART4 as 115200-N-8-N-1, Tx:PC15 Rx:PC14
 *      GPIO_QuickInit(UART4_RX_PC14_TX_PC15, 115200);
 * @endcode
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg HW_UART1
 *         @arg ...
 * @param  baudrate: uart baudrate
 * @retval None
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

//! @}

//! @defgroup UART-K_Internal_Functions
//! @{

void UART0_RX_TX_IRQHandler(void)
{
    // clear pending bit
    uint8_t ch;
    uint8_t dummy;
    // Tx
    if((UART_InstanceTable[HW_UART0]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART0]->C2 & UART_C2_TIE_MASK))
    {
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART0]->D = (uint8_t)ch;
    }
    // Rx
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
    if((UART_InstanceTable[HW_UART1]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART1]->C2 & UART_C2_TIE_MASK))
    {
        if(UART_CallBackTable[HW_UART1])
        {
            UART_CallBackTable[HW_UART1](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART1]->D = (uint8_t)ch;
    }
    //Rx
    if((UART_InstanceTable[HW_UART1]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART1]->C2 & UART_C2_RIE_MASK))
    {
        ch = (uint8_t)UART_InstanceTable[HW_UART1]->D;
        if(UART_CallBackTable[HW_UART1])
        {
            UART_CallBackTable[HW_UART1](ch, &dummy, kUART_IT_RxBTC);
        }    
    }
}

#if (!defined(MK10D5))
void UART2_RX_TX_IRQHandler(void)
{
    // clear pending bit
    uint8_t ch;
    uint8_t dummy;
    //Tx
    if((UART_InstanceTable[HW_UART2]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART2]->C2 & UART_C2_TIE_MASK))
    {
        if(UART_CallBackTable[HW_UART2])
        {
            UART_CallBackTable[HW_UART2](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART2]->D = (uint8_t)ch;
    }
    //Rx
    if((UART_InstanceTable[HW_UART2]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART2]->C2 & UART_C2_RIE_MASK))
    {
        ch = (uint8_t)UART_InstanceTable[HW_UART2]->D;
        if(UART_CallBackTable[HW_UART2])
        {
            UART_CallBackTable[HW_UART2](ch, &dummy, kUART_IT_RxBTC);
        }    
    }
}

void UART3_RX_TX_IRQHandler(void)
{
    // clear pending bit
    uint8_t ch;
    uint8_t dummy;
    //Tx
    if((UART_InstanceTable[HW_UART3]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART3]->C2 & UART_C2_TIE_MASK))
    {
        if(UART_CallBackTable[HW_UART3])
        {
            UART_CallBackTable[HW_UART3](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART3]->D = (uint8_t)ch;
    }
    //Rx
    if((UART_InstanceTable[HW_UART3]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART3]->C2 & UART_C2_RIE_MASK))
    {
        ch = (uint8_t)UART_InstanceTable[HW_UART2]->D;
        if(UART_CallBackTable[HW_UART3])
        {
            UART_CallBackTable[HW_UART3](ch, &dummy, kUART_IT_RxBTC);
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


void UART5_RX_TX_IRQHandler(void)
{
    // clear pending bit
    uint8_t ch;
    uint8_t dummy;
    //Tx
    if((UART_InstanceTable[HW_UART5]->S1 & UART_S1_TDRE_MASK) && (UART_InstanceTable[HW_UART5]->C2 & UART_C2_TIE_MASK))
    {
        if(UART_CallBackTable[HW_UART5])
        {
            UART_CallBackTable[HW_UART5](0, &ch, kUART_IT_TxBTC);
        }
        UART_InstanceTable[HW_UART5]->D = (uint8_t)ch;
    }
    //Rx
    if((UART_InstanceTable[HW_UART5]->S1 & UART_S1_RDRF_MASK) && (UART_InstanceTable[HW_UART5]->C2 & UART_C2_RIE_MASK))
    {
        ch = (uint8_t)UART_InstanceTable[HW_UART5]->D;
        if(UART_CallBackTable[HW_UART5])
        {
            UART_CallBackTable[HW_UART5](ch, &dummy, kUART_IT_RxBTC);
        }    
    }
}
#endif

//! @}

//! @}

//! @}

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
*/

