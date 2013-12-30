#include "uart.h"
#include "clock.h"
#include "gpio.h"



//!< Gloabl Const Table Defination
#define UART_IRQn_OFFSET 1
static IRQn_Type   const UART_IRQBase = UART0_IRQn;
static UART_Type * const UART_InstanceTable[] = UART_BASES;
static UART0_Type * const UART0_InstanceTable[] = UART0_BASES;
static UART_CallBackType UART_CallBackTable[ARRAY_SIZE(UART_InstanceTable) + ARRAY_SIZE(UART0_InstanceTable)] = {NULL};
static uint8_t UART_DebugInstance;

static const uint32_t SIM_UARTClockGateTable[] =
{
    SIM_SCGC4_UART0_MASK,
    SIM_SCGC4_UART1_MASK,
    SIM_SCGC4_UART2_MASK,  
};

#if (defined(UART_AUTO_LINK_CONSULT))
static uint8_t UART_getc(void)
{
    uint8_t ch;
    while(!UART_ReadByte(UART_DebugInstance, &ch));
    return ch;
}
static void UART_putc(uint8_t ch)
{
    UART_WriteByte(UART_DebugInstance,ch);
}
#endif // UART_AUTO_LINK_CONSULT

/**
 * @brief  UART init: enable UART clock Tx and Rx
 *         this function must be called before using uart
 * @param  UART_InitStruct: init type for uart
 * @retval None
 */
void UART_Init(UART_InitTypeDef * UART_InitStruct)
{
    uint16_t sbr;
    uint32_t clock;
    // open clock gate
    SIM->SCGC4 |= SIM_UARTClockGateTable[UART_InitStruct->instance];
    if(UART_InitStruct->instance == 0) // UART0 = LPUART which is different form UART1 UART2
    {
        //disable Tx Rx first
        UART0_InstanceTable[UART_InitStruct->instance]->C2 &= ~((UART0_C2_TE_MASK)|(UART0_C2_RE_MASK));
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
        UART0_InstanceTable[UART_InitStruct->instance]->C1 = 0;
        sbr = (uint16_t)((clock)/((UART_InitStruct->baudrate)*16));
        UART0_InstanceTable[UART_InitStruct->instance]->BDH &= ~(UART0_BDH_SBR_MASK);
        UART0_InstanceTable[UART_InitStruct->instance]->BDH |= ((sbr>>8) & UART0_BDH_SBR_MASK);
        UART0_InstanceTable[UART_InitStruct->instance]->BDL = (sbr & UART0_BDL_SBR_MASK);
        // enable UART Tx & Rx
        UART0_InstanceTable[UART_InitStruct->instance]->C2 |=(UART0_C2_TE_MASK  | UART0_C2_RE_MASK );
        //link debug instance
        UART_DebugInstance = UART_InitStruct->instance;
    }
    else
    {
        //get clock
        CLOCK_GetClockFrequency(kBusClock, &clock); 
        // Disable Tx and Rx first
        UART_InstanceTable[UART_InitStruct->instance-1]->C1 = 0;
        sbr = (uint16_t)((clock)/((UART_InitStruct->baudrate)*16));
        UART_InstanceTable[UART_InitStruct->instance-1]->BDH &= ~(UART_BDH_SBR_MASK);
        UART_InstanceTable[UART_InitStruct->instance-1]->BDH |= ((sbr>>8) & UART_BDH_SBR_MASK);
        UART_InstanceTable[UART_InitStruct->instance-1]->BDL = (sbr & UART_BDL_SBR_MASK);
        // enable UART Tx & Rx
        UART_InstanceTable[UART_InitStruct->instance-1]->C2 |=(UART_C2_TE_MASK  | UART_C2_RE_MASK );
    }
    //link debug instance
    UART_DebugInstance = UART_InitStruct->instance;
    // link consult
#if (defined(UART_AUTO_LINK_CONSULT))
    ConsultSelcet(UART_getc, UART_putc);
#endif
}

/**
 * @brief  UART send a byte(blocking function)
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg ...
 * @param  ch: byte to send
 * @retval None
 */
void UART_WriteByte(uint8_t instance, uint8_t ch)
{
    if(instance == 0)
    {
        while(!(UART0_InstanceTable[instance]->S1 & UART0_S1_TDRE_MASK));
        UART0_InstanceTable[instance]->D = (uint8_t)ch; 
    }
    else
    {
        while(!(UART_InstanceTable[instance-1]->S1 & UART_S1_TDRE_MASK));
        UART_InstanceTable[instance-1]->D = (uint8_t)ch;
    }

}

/**
 * @brief  UART receive a byte(none blocking function)
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg ...
 * @param  ch: pointer of byte to received
 * @retval 0:succ 1:fail
 */
uint8_t UART_ReadByte(uint8_t instance, uint8_t *ch)
{
    if(instance == 0)
    {
        if((UART0_InstanceTable[instance]->S1 & UART0_S1_RDRF_MASK) != 0)
        {
            *ch = (uint8_t)(UART0_InstanceTable[instance]->D);	
            return 0; 		  
        }
        return 1;   
    }
    else
    {
        if((UART_InstanceTable[instance-1]->S1 & UART_S1_RDRF_MASK) != 0)
        {
            *ch = (uint8_t)(UART_InstanceTable[instance-1]->D);	
            return 0; 		  
        }
        return 1;   
    }
}

 /**
 * @brief  quick init for user, do not need init struct. 
 * @code
 *      // Init UART1 as 115200-N-8-N-1, Tx:PE0 Rx:PE1
 *      GPIO_QuickInit(UART1_RX_PE01_TX_PE00, 115200);
 * @endcode
 * @param  instance: UART instance
 *         @arg HW_UART0
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
    // init pinmux and  open drain and pull up
    for(i = 0; i < pUARTxMap->io_offset; i++)
    {
        PORT_PinMuxConfig(pUARTxMap->io_instance, pUARTxMap->io_base + i, (PORT_PinMux_Type) pUARTxMap->mux); 
    }
    return pUARTxMap->ip_instance;
}

/**
 * @brief  UART DMA and Interrupt config
 * @param  instance: UART instance
 *         @arg HW_UART0
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
    if(instance == 0)
    {
        switch(config)
        {
            case kUART_ITDMA_Disable:
                NVIC_DisableIRQ((IRQn_Type)(UART_IRQBase + instance*UART_IRQn_OFFSET));
                UART0_InstanceTable[instance]->C2 &= ~UART0_C2_TIE_MASK;
                UART0_InstanceTable[instance]->C2 &= ~UART0_C2_RIE_MASK;
                UART0_InstanceTable[instance]->C4 &= ~UART0_C5_TDMAE_MASK;
                UART0_InstanceTable[instance]->C4 &= ~UART0_C5_RDMAE_MASK;
                break;
            case kUART_IT_TxBTC:
                (ENABLE == newState)?(UART0_InstanceTable[instance]->C2 |= UART0_C2_TIE_MASK):(UART0_InstanceTable[instance]->C2 &= ~UART0_C2_TIE_MASK);
                if(ENABLE == newState)
                {
                    NVIC_EnableIRQ((IRQn_Type)(UART_IRQBase + instance*UART_IRQn_OFFSET));
                }
                break;
            case kUART_DMA_TxBTC:
                (ENABLE == newState)?(UART0_InstanceTable[instance]->C5 |= UART0_C5_TDMAE_MASK):(UART0_InstanceTable[instance]->C4 &= ~UART0_C5_TDMAE_MASK);
                break;
            case kUART_IT_RxBTC:
                (ENABLE == newState)?(UART0_InstanceTable[instance]->C2 |= UART0_C2_RIE_MASK):(UART0_InstanceTable[instance]->C2 &= ~UART0_C2_RIE_MASK);
                if(ENABLE == newState)
                {
                    NVIC_EnableIRQ((IRQn_Type)(UART_IRQBase + instance*UART_IRQn_OFFSET));
                }
                break;
            case kUART_DMA_RxBTC:
                (ENABLE == newState)?(UART0_InstanceTable[instance]->C5 |= UART0_C5_RDMAE_MASK):(UART0_InstanceTable[instance]->C5 &= ~UART0_C5_RDMAE_MASK);
                break;
            default:
                break;
        } 
    }
    else
    {
        switch(config)
        {
            case kUART_ITDMA_Disable:
                NVIC_DisableIRQ((IRQn_Type)(UART_IRQBase + instance*UART_IRQn_OFFSET));
                UART_InstanceTable[instance-1]->C2 &= ~UART_C2_TIE_MASK;
                UART_InstanceTable[instance-1]->C2 &= ~UART_C2_RIE_MASK;
                UART_InstanceTable[instance-1]->C4 &= ~UART_C4_TDMAS_MASK;
                UART_InstanceTable[instance-1]->C4 &= ~UART_C4_RDMAS_MASK;
                break;
            case kUART_IT_TxBTC:
                (ENABLE == newState)?(UART_InstanceTable[instance-1]->C2 |= UART_C2_TIE_MASK):(UART_InstanceTable[instance-1]->C2 &= ~UART_C2_TIE_MASK);
                if(ENABLE == newState)
                {
                    NVIC_EnableIRQ((IRQn_Type)(UART_IRQBase + instance*UART_IRQn_OFFSET));
                }
                break;
            case kUART_DMA_TxBTC:
                (ENABLE == newState)?(UART_InstanceTable[instance-1]->C4 |= UART_C4_TDMAS_MASK):(UART_InstanceTable[instance-1]->C4 &= ~UART_C4_TDMAS_MASK);
                break;
            case kUART_IT_RxBTC:
                (ENABLE == newState)?(UART_InstanceTable[instance-1]->C2 |= UART_C2_RIE_MASK):(UART_InstanceTable[instance-1]->C2 &= ~UART_C2_RIE_MASK);
                if(ENABLE == newState)
                {
                    NVIC_EnableIRQ((IRQn_Type)(UART_IRQBase + instance*UART_IRQn_OFFSET));
                }
                break;
            case kUART_DMA_RxBTC:
                (ENABLE == newState)?(UART_InstanceTable[instance-1]->C4 |= UART_C4_RDMAS_MASK):(UART_InstanceTable[instance-1]->C4 &= ~UART_C4_RDMAS_MASK);
                break;
            default:
                break;
        }
    }

}

/**
 * @brief  Install UART Tx Rx Callback function
 *         this function should be called before ITDMAConfig function
 * @param  instance: UART instance
 *         @arg HW_UART0
 *         @arg ...
 * @param  AppCBFun: pointer of callback function
 * @retval 0:succ 1:fail
 */
void UART_CallbackInstall(uint8_t instance, UART_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        UART_CallBackTable[instance] = AppCBFun;
    }
}

void UART0_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART0->S1 & UART_S1_TDRE_MASK) && (UART0->C2 & UART0_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](0, &ch, kUART_IT_TxBTC);
        }
        UART0->D = (uint8_t)ch;
   }
   //Rx
   if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART0_C2_RIE_MASK))
   {
        ch = (uint8_t)UART0->D;
        if(UART_CallBackTable[HW_UART0])
        {
            UART_CallBackTable[HW_UART0](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}

void UART1_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART1->S1 & UART_S1_TDRE_MASK) && (UART1->C2 & UART_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART1-1])
        {
            UART_CallBackTable[HW_UART1](0, &ch, kUART_IT_TxBTC);
        }
        UART1->D = (uint8_t)ch;
   }
   //Rx
   if((UART1->S1 & UART_S1_RDRF_MASK) && (UART1->C2 & UART_C2_RIE_MASK))
   {
        ch = (uint8_t)UART1->D;
        if(UART_CallBackTable[HW_UART1])
        {
            UART_CallBackTable[HW_UART1](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}

void UART2_IRQHandler(void)
{
    // clear pending bit
   uint8_t ch;
   uint8_t dummy;
   //Tx
   if((UART2->S1 & UART_S1_TDRE_MASK) && (UART2->C2 & UART_C2_TIE_MASK))
   {
        if(UART_CallBackTable[HW_UART2])
        {
            UART_CallBackTable[HW_UART2](0, &ch, kUART_IT_TxBTC);
        }
        UART2->D = (uint8_t)ch;
   }
   //Rx
   if((UART2->S1 & UART_S1_RDRF_MASK) && (UART2->C2 & UART_C2_RIE_MASK))
   {
        ch = (uint8_t)UART2->D;
        if(UART_CallBackTable[HW_UART2])
        {
            UART_CallBackTable[HW_UART2](ch, &dummy, kUART_IT_RxBTC);
        }    
   }
}

/*
static const QuickInit_Type UART_QuickInitTable[] =
{
    { 1, 4, 3, 0, 2, 0}, //UART1_RX_PE01_TX_PE00  3
    { 1, 0, 3,18, 2, 0}, //UART1_RX_PA18_TX_PA19  3
    { 1, 2, 3, 3, 2, 0}, //UART1_RX_PC03_TX_PC04  3
    { 2, 4, 4,22, 2, 0}, //UART2_RX_PE23_TX_PE22  4
    { 2, 3, 3, 2, 2, 0}, //UART2_RX_PD02_TX_PD03  3
    { 2, 3, 3, 4, 2, 0}, //UART2_RX_PD04_TX_PD05  3
};
*/


