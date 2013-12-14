/**
  ******************************************************************************
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.25
  * @brief   超核K60固件库 GPIO 函数库 API函数
  ******************************************************************************
  */
#include "gpio.h"


//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup GPIO
//! @brief GPIO driver modules
//! @{


//! @defgroup GPIO_Exported_Functions
//! @{


//!< Leagacy Support for Kineis Z Version
#if (!defined(GPIO_BASES))

    #if (defined(MK60DZ10))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #elif
		;
    #endif

#endif

//!< IRQTable Definitaion
IRQn_Type   const GPIO_IRQBase = PORTA_IRQn;
GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType  gGPIOCallBackFun;

//!< GPIO & PORT Clock Gate Table
const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK,
};

State_Type PORT_PinMuxConfig(GPIO_Instance_Type instance, uint8_t pinIndex, PORT_PinMux_Type pinMux)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    if(instance >= ARRAY_SIZE(PORT_InstanceTable))
		{
        return kStatusInvalidArgument;
		}
		PORT_InstanceTable[instance]->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);
		PORT_InstanceTable[instance]->PCR[pinIndex] |=  PORT_PCR_MUX(pinMux);
		return kStatus_Success;
}

State_Type PORT_PinConfig(GPIO_Instance_Type instance, uint8_t pinIndex, PORT_Pull_Type pull, FunctionalState newState)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
		(newState == ENABLE) ? (PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_ODE_MASK);
    switch(pull)
		{
        case kPullDisabled:
            PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PE_MASK;
            break;
        case kPullUp:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PS_MASK;
            break;
        case kPullDown:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            return kStatusInvalidArgument;
            break;
		}
		return kStatus_Success;
}

State_Type GPIO_PinConfig(GPIO_Instance_Type instance, uint8_t pinIndex, GPIO_PinConfig_Type pull)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
		if(pull >= kPinConfigNameCount)
		{
        return kStatusInvalidArgument;
		}
    (pull == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pinIndex));
		return kStatus_Success;
}



	/**
  * @brief  Initializes the GPIOx peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
State_Type GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    //param check
    if(GPIO_InitStruct->instance >= ARRAY_SIZE(GPIO_InstanceTable))
		{
        return kStatusInvalidArgument;
		}
    if(GPIO_InitStruct->pinx >= kGPIO_PinNameCount)
		{
        return kStatusInvalidArgument;
		}
    if(GPIO_InitStruct->mode >= kGPIO_ModeNameCount)
		{
        return kStatusInvalidArgument;
		}
		//config state
		switch(GPIO_InitStruct->mode)
		{
        case kGPIO_Mode_IFT:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInpput);
            break;
        case kGPIO_Mode_IPD:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDown, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInpput);
            break;
        case kGPIO_Mode_IPU:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInpput);
            break;
        case kGPIO_Mode_OOD:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp, ENABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        case kGPIO_Mode_OPP:
            PORT_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled, DISABLE);
						GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        default:
            break;					
    }
    //config pinMux
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

State_Type GPIO_QuickInit(GPIO_Instance_Type instance, GPIO_Pin_Type pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
		GPIO_InitStruct1.instance = instance;
		GPIO_InitStruct1.mode = mode;
		GPIO_InitStruct1.pinx = pinx;
		return GPIO_Init(&GPIO_InitStruct1);
}

	/**
  * @brief  GPIO write a single bit to any GPIO pin
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @param  BitVal : bit value
	*         @arg Bit_RESET : low state
	*         @arg Bit_SET   : high state
  * @retval None
  */
void GPIO_WriteBit(GPIO_Instance_Type instance, uint8_t pinIndex, uint8_t data)
{
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pinIndex));
}

uint8_t GPIO_ReadBit(GPIO_Instance_Type instance, uint8_t pinIndex)
{
    if(((GPIO_InstanceTable[instance]->PDDR) >> pinIndex) & 0x01)
		{
        //output
        return ((GPIO_InstanceTable[instance]->PDOR >> pinIndex) & 0x01);
		}
		else
		{
			//input
        return ((GPIO_InstanceTable[instance]->PDIR >> pinIndex) & 0x01);
		}
}
	/**
  * @brief  Toggle a GPIO single bit
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @retval None
  */
void GPIO_ToggleBit(GPIO_Instance_Type instance, uint8_t pinIndex)
{
    GPIO_InstanceTable[instance]->PTOR |= (1 << pinIndex);
}


	/**
  * @brief  Enable or disable GPIO's DMA support
  * @code
  *      // Check if transmit buffer is empty.
  *      if (spi_hal_is_transmit_buffer_empty(0))
  *      {
  *          // Buffer has room, so write the next data value.
  *          spi_hal_write_data(0, byte);
  *      }
  * @endcode
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_DMASelect_TypeDef: DMA trigger source select
	*         @arg kGPIO_DMA_Rising
	*         @arg kGPIO_DMA_Falling
	*         @arg kGPIO_DMA_RisingAndFalling
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @param  NewState : disable or enable
  *         @arg ENABLE
  *         @arg DISABLE
  * @retval None
  */
void GPIO_DMACmd(GPIO_Instance_Type instance, uint8_t pinIndex, GPIO_DMA_Type dmaReq, FunctionalState newState)
{
    uint8_t i;
    PORT_InstanceTable[pinIndex]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
		if(ENABLE == newState)
		{
			PORT_InstanceTable[pinIndex]->PCR[pinIndex] |= PORT_PCR_IRQC(dmaReq);
		}
}


uint32_t GPIO_ReadByte(GPIO_Instance_Type instance, uint8_t pinIndex)
{
    return (GPIO_InstanceTable[instance]->PDIR);
}
/**
 * @brief  write GPIO 32 bit data
 * @param  GPIOx: pointer to a GPIO_Type
 *         @arg PTA: A Port
 *         @arg PTB: B Port
 *         @arg PTC: C Port
 *         @arg PTD: D Port
 *         @arg PTE: E Port 
 * @param  PortVal: 32bit data value
 * @retval None
 */
void GPIO_WriteByte(GPIO_Instance_Type instance, uint8_t pinIndex, uint32_t data)
{
    GPIO_InstanceTable[instance]->PDOR = data;
}


State_Type GPIO_CallBackInstall(GPIO_CallBackType AppCallBack)
{
    if(AppCallBack != NULL)
		{
        gGPIOCallBackFun = AppCallBack;
		}
//	AppCallBack
   // NVIC_EnableIRQ((IRQn_Type)(GPIO_IRQBase + instance));
}


void PORTA_IRQHandler(void)
{
    gGPIOCallBackFun(HW_GPIOA, 1);
}

void PORTB_IRQHandler(void)
{
    gGPIOCallBackFun(HW_GPIOB, 1);
}

void PORTC_IRQHandler(void)
{
    gGPIOCallBackFun(HW_GPIOC, 1);
}

void PORTD_IRQHandler(void)
{
    gGPIOCallBackFun(HW_GPIOD, 1);
}

void PORTE_IRQHandler(void)
{
    gGPIOCallBackFun(HW_GPIOE, 1);
}

void PORTF_IRQHandler(void)
{
    gGPIOCallBackFun(HW_GPIOF, 1);
}









ITStatus GPIO_GetITStates(GPIO_Type *GPIOx,GPIO_Pin_Type GPIO_Pin)
{
    PORT_Type *PORTx = NULL;
    //开端口时钟
    switch((uint32_t)GPIOx)
    {
        case PTA_BASE:PORTx = PORTA;break;
        case PTB_BASE:PORTx = PORTB;break;
        case PTC_BASE:PORTx = PORTC;break;
        case PTD_BASE:PORTx = PORTD;break;
        case PTE_BASE:PORTx = PORTE;break;
        default : break;
    } 
    //返回标志位
    if(PORTx->ISFR & (1<<GPIO_Pin))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}
	/**
  * @brief  clear interrupt status flag of GPIO peripheral
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0    
  * @retval None
  */
void GPIO_ClearITPendingBit(GPIO_Type *GPIOx,uint16_t GPIO_Pin)
{
    PORT_Type *PORTx = NULL;
    //开端口时钟
    switch((uint32_t)GPIOx)
    {
        case PTA_BASE:PORTx=PORTA;SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;break; //开启PORTA口使能时钟，在设置前首先开启使能时钟参见k10手册268页，
        case PTB_BASE:PORTx=PORTB;SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;break;	//开启PORTB口使能时钟
        case PTC_BASE:PORTx=PORTC;SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;break;	//开启PORTC口使能时钟
        case PTD_BASE:PORTx=PORTD;SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;break;	//开启PORTD口使能时钟
        case PTE_BASE:PORTx=PORTE;SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;break;	//开启PORTE口使能时钟
        default : break;
    } 
    PORTx->ISFR |= (1<<GPIO_Pin);
}
	/**
  * @brief  clear all interrupt status flag of GPIO peripheral
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @retval None
  */
void GPIO_ClearAllITPendingBit(GPIO_Type *GPIOx)
{
    PORT_Type *PORTx = NULL;
    //开端口时钟
    switch((uint32_t)GPIOx)
    {
        case PTA_BASE:PORTx=PORTA;SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;break; //开启PORTA口使能时钟，在设置前首先开启使能时钟参见k10手册268页，
        case PTB_BASE:PORTx=PORTB;SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;break;	//开启PORTB口使能时钟
        case PTC_BASE:PORTx=PORTC;SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;break;	//开启PORTC口使能时钟
        case PTD_BASE:PORTx=PORTD;SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;break;	//开启PORTD口使能时钟
        case PTE_BASE:PORTx=PORTE;SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;break;	//开启PORTE口使能时钟
        default : break;
    } 
    PORTx->ISFR |= 0xFFFFFFFF;
}
	/**
  * @brief  default GPIO struct init
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */







void GPIO_ITConfig(GPIO_Type* GPIOx, GPIO_ITSelect_TypeDef GPIO_IT, GPIO_Pin_Type GPIO_Pin, FunctionalState NewState)
{
    PORT_Type *PORTx = NULL;
    IRQn_Type  IRQn;
    switch((uint32_t)GPIOx)
    {
        case (uint32_t)PTA_BASE:
            PORTx = PORTA;
            IRQn = PORTA_IRQn;
            break; 
        case (uint32_t)PTB_BASE:
            PORTx = PORTB; 
            IRQn = PORTB_IRQn;
            break;	
        case PTC_BASE:
            PORTx = PORTC; 
            IRQn = PORTC_IRQn; 
            break;	
        case PTD_BASE:
            PORTx = PORTD; 
            IRQn = PORTD_IRQn; 
            break;	
        case PTE_BASE:
            PORTx = PORTE; 
            IRQn = PORTE_IRQn; 
            break;	
        default : break;
    } 
    PORTx->PCR[(uint8_t)GPIO_Pin] &= ~PORT_PCR_IRQC_MASK;
    (ENABLE == NewState)?(PORTx->PCR[(uint8_t)GPIO_Pin] |= PORT_PCR_IRQC((uint8_t)GPIO_IT)):(PORTx->PCR[(uint8_t)GPIO_Pin] &= ~PORT_PCR_IRQC_MASK);
		(ENABLE == NewState)?(NVIC_EnableIRQ(IRQn)):(NVIC_DisableIRQ(IRQn));
}


//! @}

//! @}

//! @}
