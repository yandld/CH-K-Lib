#include "lpuart.h"
#include "clock.h"
#include "gpio.h"

//!< Gloabl Const Table Defination
static IRQn_Type   const LPUART_IRQBase = UART0_IRQn;
static UART0_Type * const LPUART_InstanceTable[] = UART0_BASES;
static LPUART_CallBackType LPUART_CallBackTable[ARRAY_SIZE(LPUART_InstanceTable)] = {NULL};

static const uint32_t SIM_LPUARTClockGateTable[] =
{
    SIM_SCGC4_UART0_MASK,
};


void LPUART_Init(LPUART_InitTypeDef * LPUART_InitStruct)
{
    uint16_t sbr;
    uint32_t clock;
    // open clock gate
    SIM->SCGC4 |= SIM_LPUARTClockGateTable[LPUART_InitStruct->instance];
    //disable Tx Rx first
    LPUART_InstanceTable[LPUART_InitStruct->instance]->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);  //use PLL/2 or FLL
    //chose PLL or FLL to use. this driver only support FLL or PLL output
    if(MCG->C6 & MCG_C6_PLLS_MASK) //PLL is selected
    {
        
        SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
        CLOCK_GetClockFrequency(kCoreClock, &clock);
    }
    else //FLL is sleected
    {
        SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
        CLOCK_GetClockFrequency(kCoreClock, &clock);	
    }
    // Disable Tx and Rx first
    LPUART_InstanceTable[LPUART_InitStruct->instance]->C1 = 0;
    sbr = (uint16_t)((clock)/((LPUART_InitStruct->baudrate)*16));
    LPUART_InstanceTable[LPUART_InitStruct->instance]->BDH &= ~(UART_BDH_SBR_MASK);
    LPUART_InstanceTable[LPUART_InitStruct->instance]->BDH |= ((sbr>>8) & UART_BDH_SBR_MASK);
    LPUART_InstanceTable[LPUART_InitStruct->instance]->BDL = (sbr & UART_BDL_SBR_MASK);
    // enable UART Tx & Rx
    LPUART_InstanceTable[LPUART_InitStruct->instance]->C2 |=(UART_C2_TE_MASK  | UART_C2_RE_MASK );
}

/**
 * @brief  UART send a byte(blocking function)
 * @param  instance: GPIO instance
 *         @arg HW_LPUART0
 * @param  ch: byte to send
 * @retval None
 */
void LPUART_SendByte(uint8_t instance, uint8_t ch)
{
    while(!(LPUART_InstanceTable[instance]->S1 & UART_S1_TDRE_MASK));
    LPUART_InstanceTable[instance]->D = (uint8_t)ch;
}


/**
 * @brief  UART receive a byte(none blocking function)
 * @param  instance: GPIO instance
 *         @arg HW_LPUART0
 * @param  ch: pointer of byte to received
 * @retval 0:succ 1:fail
 */
uint8_t LPUART_ReadByte(uint8_t instance, uint8_t *ch)
{
    if((LPUART_InstanceTable[instance]->S1 & UART_S1_RDRF_MASK) != 0)
    {
        *ch = (uint8_t)(UART0->D);	
        return 0; 		  
    }
    return 1;
}


void LPUART_putstr(uint8_t instance, const char *str)
{
    while(*str != '\0')
    {
        LPUART_SendByte(instance, *str++);
    }
}

//内部函数为实现UART_printf
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
	LPUART_SendByte(0, ntab[m]);
}
/***********************************************************************************************
 功能：UART 格式化输出
 形参：fmt 输入字符串指针          
 返回：0
 详解：类似于C标准库中的UART_printf 但是只支持 %d %l %o %x %s
************************************************************************************************/
void UART_printf(char *fmt, ...)
{
    char c;
    unsigned int *adx = (unsigned int*)(void*)&fmt + 1;
_loop:
    while((c = *fmt++) != '%')
		{
        if (c == '\0') return;
        LPUART_SendByte(0,c);
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
			LPUART_putstr(0, (char*)*adx);
    }
    adx++;
    goto _loop;
}




void LPUART_ITDMAConfig(uint8_t instance, LPUART_ITDMAConfig_Type config, FunctionalState newState)
{
    // disable interrupt and dma first
    NVIC_DisableIRQ((IRQn_Type)(LPUART_IRQBase + instance));
    // disable DMA and IT
    switch(config)
    {
        case kLPUART_ITDMA_Disable:
            break;
        case kLPUART_IT_TxBTC:
            (ENABLE == newState)?(LPUART_InstanceTable[instance]->C2 |= UART0_C2_TIE_MASK):(LPUART_InstanceTable[instance]->C2 &= ~UART0_C2_TIE_MASK);
            break;
        case kLPUART_DMA_TxBTC:
            break;
        case kLPUART_IT_RxBTC:
            (ENABLE == newState)?(LPUART_InstanceTable[instance]->C2 |= UART0_C2_RIE_MASK):(LPUART_InstanceTable[instance]->C2 &= ~UART0_C2_RIE_MASK);
            break;
        case kLPUART_DMA_RxBTC:
            break;
        default:
            break;
    }
    NVIC_EnableIRQ((IRQn_Type)(LPUART_IRQBase + instance));
}

// this function should be called before ITDMAConfig function
void LPUART_CallbackInstall(uint8_t instance, LPUART_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        LPUART_CallBackTable[instance] = AppCBFun;
    }
}

uint8_t LPUART_QuickInit(uint32_t LPUARTxMAP, uint32_t baudrate)
{
    uint8_t i;
    LPUART_InitTypeDef LPUART_InitStruct1;
    QuickInit_Type * pLPUARTxMap = (QuickInit_Type*)&(LPUARTxMAP);
    LPUART_InitStruct1.baudrate = baudrate;
    LPUART_InitStruct1.instance = pLPUARTxMap->ip_instance;
    LPUART_Init(&LPUART_InitStruct1);
    // init pinmux and  open drain and pull up
    for(i = 0; i < pLPUARTxMap->io_offset; i++)
    {
        PORT_PinMuxConfig(pLPUARTxMap->io_instance, pLPUARTxMap->io_base + i, (PORT_PinMux_Type) pLPUARTxMap->mux); 
    }
    return pLPUARTxMap->ip_instance;
}



void UART0_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((LPUART_InstanceTable[HW_LPUART0]->S1 & UART_S1_TDRE_MASK) && (LPUART_InstanceTable[HW_LPUART0]->C2 & UART0_C2_TIE_MASK))
   {
        if(LPUART_CallBackTable[HW_LPUART0])
        {
            LPUART_CallBackTable[HW_LPUART0](0, &ch, kLPUART_IT_TxBTC);
        }
        LPUART_InstanceTable[HW_LPUART0]->D = (uint8_t)ch;
   }
   //Rx
   if((LPUART_InstanceTable[HW_LPUART0]->S1 & UART_S1_RDRF_MASK) && (LPUART_InstanceTable[HW_LPUART0]->C2 & UART0_C2_RIE_MASK))
   {
        ch = (uint8_t)LPUART_InstanceTable[HW_LPUART0]->D;
        if(LPUART_CallBackTable[HW_LPUART0])
        {
            LPUART_CallBackTable[HW_LPUART0](ch, &dummy, kLPUART_IT_RxBTC);
        }    
   }
}

/*
static const QuickInit_Type LPUART_QuickInitTable[] =
{
    { 0, 4, 4,20, 2, 0}, //UART0_RX_PE21_TX_PE20
    { 0, 0, 2, 1, 2, 0}, //UART0_RX_PA01_TX_PA02
    { 0, 0, 3,14, 2, 0}, //UART0_RX_PA15_TX_PA14
    { 0, 1, 3,16, 2, 0}, //UART0_RX_PB16_TX_PB17
    { 0, 3, 3, 6, 2, 0}, //UART0_RX_PD06_TX_PD07
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

