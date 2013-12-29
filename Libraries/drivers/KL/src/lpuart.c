#include "lpuart.h"
#include "clock.h"
#include "gpio.h"


//!< Gloabl Const Table Defination
#define LPUART_IRQn_OFFSET 1
static IRQn_Type   const LPUART_IRQBase = UART0_IRQn;
static UART0_Type * const LPUART_InstanceTable[] = UART0_BASES;
static LPUART_CallBackType LPUART_CallBackTable[ARRAY_SIZE(LPUART_InstanceTable)] = {NULL};
static uint8_t UART_DebugInstance;

static const uint32_t SIM_LPUARTClockGateTable[] =
{
    SIM_SCGC4_UART0_MASK,
};

#if (defined(UART_AUTO_LINK_CONSULT))
static uint8_t LPUART_getc(void)
{
    uint8_t ch;
    while(!LPUART_ReadByte(UART_DebugInstance, &ch));
    return ch;
}
static void LPUART_putc(uint8_t ch)
{
    LPUART_WriteByte(UART_DebugInstance,ch);
}
#endif // UART_AUTO_LINK_CONSULT

/**
 * @brief  LPUART init: enable UART clock Tx and Rx
 *         this function must be called before using uart
 * @param  UART_InitStruct: init type for uart
 * @retval None
 */
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
    //link debug instance
    UART_DebugInstance = LPUART_InitStruct->instance;
    // link consult
#if (defined(UART_AUTO_LINK_CONSULT))
    ConsultSelcet(LPUART_getc, LPUART_putc);
#endif
}

/**
 * @brief  LPUART send a byte(blocking function)
 * @param  instance: UART instance
 *         @arg HW_LPUART0
 *         @arg ...
 * @param  ch: byte to send
 * @retval None
 */
void LPUART_WriteByte(uint8_t instance, uint8_t ch)
{
    while(!(LPUART_InstanceTable[instance]->S1 & UART_S1_TDRE_MASK));
    LPUART_InstanceTable[instance]->D = (uint8_t)ch;
}

/**
 * @brief  LPUART receive a byte(none blocking function)
 * @param  instance: UART instance
 *         @arg HW_LPUART0
 *         @arg ...
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



/**
 * @brief  LPUART DMA and Interrupt config
 * @param  instance: UART instance
 *         @arg HW_LPUART0
 *         @arg ...
 * @param  config: config, see LPUART_ITDMAConfig_Type for more details
 * @param  newState:
 *         @arg ENABLE
 *         @arg DISABLE
 * @param  ch: pointer of byte to received
 * @retval None
 */
void LPUART_ITDMAConfig(uint8_t instance, LPUART_ITDMAConfig_Type config, FunctionalState newState)
{
    switch(config)
    {
        case kLPUART_ITDMA_Disable:
            NVIC_DisableIRQ((IRQn_Type)(LPUART_IRQBase + instance*LPUART_IRQn_OFFSET));
            LPUART_InstanceTable[instance]->C2 &= ~UART0_C2_TIE_MASK;
            LPUART_InstanceTable[instance]->C2 &= ~UART0_C2_RIE_MASK;
            LPUART_InstanceTable[instance]->C4 &= ~UART_C4_TDMAS_MASK;
            LPUART_InstanceTable[instance]->C4 &= ~UART_C4_RDMAS_MASK;
            break;
        case kLPUART_IT_TxBTC:
            (ENABLE == newState)?(LPUART_InstanceTable[instance]->C2 |= UART0_C2_TIE_MASK):(LPUART_InstanceTable[instance]->C2 &= ~UART0_C2_TIE_MASK);
            if(ENABLE == newState)
            {
                NVIC_EnableIRQ((IRQn_Type)(LPUART_IRQBase + instance*LPUART_IRQn_OFFSET));
            }
            break;
        case kLPUART_DMA_TxBTC:
            (ENABLE == newState)?(LPUART_InstanceTable[instance]->C4 |= UART_C4_TDMAS_MASK):(LPUART_InstanceTable[instance]->C4 &= ~UART_C4_TDMAS_MASK);
            break;
        case kLPUART_IT_RxBTC:
            (ENABLE == newState)?(LPUART_InstanceTable[instance]->C2 |= UART0_C2_RIE_MASK):(LPUART_InstanceTable[instance]->C2 &= ~UART0_C2_RIE_MASK);
            if(ENABLE == newState)
            {
                NVIC_EnableIRQ((IRQn_Type)(LPUART_IRQBase + instance*LPUART_IRQn_OFFSET));
            }
            break;
        case kLPUART_DMA_RxBTC:
            (ENABLE == newState)?(LPUART_InstanceTable[instance]->C4 |= UART_C4_RDMAS_MASK):(LPUART_InstanceTable[instance]->C4 &= ~UART_C4_RDMAS_MASK);
            break;
        default:
            break;
    }
}
/**
 * @brief  Install LPUART Tx Rx Callback function
 *         this function should be called before ITDMAConfig function
 * @param  instance: UART instance
 *         @arg HW_LPUART0
 *         @arg ...
 * @param  AppCBFun: pointer of callback function
 * @retval 0:succ 1:fail
 */
void LPUART_CallbackInstall(uint8_t instance, LPUART_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        LPUART_CallBackTable[instance] = AppCBFun;
    }
}
 /**
 * @brief  quick init for user, do not need init struct. 
 * @code
 *      // Init UART0 as 115200-N-8-N-1, Tx:PE20 Rx:PE21
 *      GPIO_QuickInit(UART0_RX_PE21_TX_PE20, 115200);
 * @endcode
 * @param  instance: LPUART instance
 *         @arg HW_LPUART0
 *         @arg ...
 * @param  baudrate: uart baudrate
 * @retval None
 */
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

*/

