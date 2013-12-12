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
#include "pinmux.h"


//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup GPIO
//! @brief GPIO driver modules
//! @{


//! @defgroup GPIO_Exported_Functions
//! @{

#if (!defined(GPIO_BASES))

    #if (defined(MK60DZ10))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #elif
		
    #endif

#endif


GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
PORT_Type * const PORT_InstanceTable[] = PORT_BASES;

const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK
};

State_Type PORT_PinMuxConfig(GPIO_Instance_Type instance, uint8_t pinIndex, PORT_PinMux_Type pinMux)
{
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
		//
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

	/**
  * @brief  Initializes the GPIOx peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
	/*
void GPIO_Init(GPIO_InitTypeDef* GPIO_InitStruct)
{
    GPIO_Type *GPIOx = NULL;
    PORT_Type *PORTx = NULL;
    //param check
    assert_param(IS_GPIO_ALL_PERIPH(GPIO_InitStruct->GPIOx));
    assert_param(IS_GPIO_BIT_ACTION(GPIO_InitStruct->GPIO_InitState )); 
    GPIOx = GPIO_InitStruct->GPIOx;
    //Open clock gate
    switch((uint32_t)GPIOx)
    {
        case PTA_BASE:
            PORTx = PORTA;
            SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
            break;
        case PTB_BASE:
            PORTx = PORTB;
            SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
            break;
        case PTC_BASE:
            PORTx = PORTC;
            SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
            break;
        case PTD_BASE:
            PORTx = PORTD;
            SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
            break;
        case PTE_BASE:
            PORTx = PORTE;
            SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
            break;
        default : 
            break;
    } 
    //set pinmux into GPIO mode
    PORTx->PCR[GPIO_InitStruct->GPIO_Pin] &= ~(PORT_PCR_MUX_MASK);    
    PORTx->PCR[GPIO_InitStruct->GPIO_Pin] |= PORT_PCR_MUX(1); 
    //input or output
    if((GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_OOD) || (GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_OPP))
    {
        //set pin into putout
        GPIOx->PDDR |= (1<<(GPIO_InitStruct->GPIO_Pin));	
        //disable pull select
        PORTx->PCR[(GPIO_InitStruct->GPIO_Pin)]&=~(PORT_PCR_PE_MASK); 
        //output low or high
        (Bit_SET == GPIO_InitStruct->GPIO_InitState)?(GPIOx->PDOR |= (1<<(GPIO_InitStruct->GPIO_Pin))):(GPIOx->PDOR &= ~(1<<(GPIO_InitStruct->GPIO_Pin)));
			  //open drian output or push pull output
        if(GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_OOD)
        {
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]|= PORT_PCR_ODE_MASK;
        }
        else if (GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_OPP)
        {
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]&= ~PORT_PCR_ODE_MASK;
        }
    }
    //input mode
    else if ((GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_IFT) || (GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_IPD) || (GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_IPU))
    {
			  //set pin to input
        GPIOx->PDDR &= ~(1<<(GPIO_InitStruct->GPIO_Pin));		
        if(GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_IFT)
        {
            //disable pull resister
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]&=~PORT_PCR_PE_MASK; 
        }
        else if (GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_IPD)
        {
            //pull down
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]|= PORT_PCR_PE_MASK;
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]&= ~PORT_PCR_PS_MASK;
        }
        else if (GPIO_InitStruct->GPIO_Mode == kGPIO_Mode_IPU)
        {
            //pull up
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]|= PORT_PCR_PE_MASK;
            PORTx->PCR[GPIO_InitStruct->GPIO_Pin]|= PORT_PCR_PS_MASK;
        }
    }
}
*/

	/**
  * @brief  Get interrupt status flag of GPIO peripheral
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0    
  * @retval None
  */
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
void GPIO_WriteBit(GPIO_Type *GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{	 
    //检测参数
    assert_param(IS_GPIO_BIT_ACTION(BitVal)); 
	
    if (BitVal != Bit_RESET)
    {
        GPIOx->PSOR |= (1<<GPIO_Pin);
    }
    else
    {
        GPIOx->PCOR |= (1<<GPIO_Pin);
    }
}
	/**
  * @brief  GPIO write a single bit to any GPIO pin to hight state
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @retval None
  */
void GPIO_SetBits(GPIO_Type* GPIOx, uint16_t GPIO_Pin)
{
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    GPIOx->PSOR |= (1<<GPIO_Pin);
}
	/**
  * @brief  GPIO write a single bit to any GPIO pin to low state
  * @param  GPIOx: pointer to a GPIO_Type
  *         @arg PTA: A Port
  *         @arg PTB: B Port
  *         @arg PTC: C Port
  *         @arg PTD: D Port
	*         @arg PTE: E Port 
  * @param  GPIO_Pin: GPIO pin:  eg : kGPIO_Pin_0   
  * @retval None
  */
void GPIO_ResetBits(GPIO_Type* GPIOx, uint16_t GPIO_Pin)
{
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    GPIOx->PCOR |= (1<<GPIO_Pin);
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
void GPIO_ToggleBit(GPIO_Type *GPIOx, uint16_t GPIO_Pin)
{
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    
    GPIOx->PTOR |= (1<<GPIO_Pin);	//将引脚的电平进行翻转，参考k10手册1483页
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
void GPIO_WriteData(GPIO_Type *GPIOx,uint32_t PortVal)
{
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    GPIOx->PDOR = PortVal;
}
/***********************************************************************************************
 功能：读取一个已经设置为输出的IO Pin的 电平
 形参：GPIOx:
			 @arg PTA:A端口
			 @arg PTB:B端口
			 @arg PTC:C端口
			 @arg PTD:D端口
			 @arg PTE:E端口
		 	 GPIO_Pin：
			 @arg 0-31: 端口号
 返回：端口电平
 详解：实际上就是读取PDOR的值
************************************************************************************************/
uint8_t GPIO_ReadOutputDataBit(GPIO_Type* GPIOx, uint16_t GPIO_Pin)
{
    uint8_t bitstatus = 0x00;
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
    if(((GPIOx->PDOR >> GPIO_Pin) & 1 ) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;
    }
    return bitstatus;
}
/***********************************************************************************************
 功能：读取一个已经设置为输出的IO的 电平
 形参：GPIOx:
			 @arg PTA:A端口
			 @arg PTB:B端口
			 @arg PTC:C端口
			 @arg PTD:D端口
			 @arg PTE:E端口
 返回：端口电平
 详解：实际上就是读取PDOR的值
************************************************************************************************/
uint32_t GPIO_ReadOutputData(GPIO_Type* GPIOx)
{
  //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    
    return ((uint16_t)GPIOx->PDOR);
}
/***********************************************************************************************
 功能：读取一个IO端口的某一个PIN的电平
 形参：GPIOx:
			 @arg PTA:A端口
			 @arg PTB:B端口
			 @arg PTC:C端口
			 @arg PTD:D端口
			 @arg PTE:E端口
			 GPIO_Pin:
			 @arg 0-31 数字
 返回：逻辑电平 0 或者 1
 详解：必须先设置端口为输入端口
************************************************************************************************/
uint8_t GPIO_ReadInputDataBit(GPIO_Type* GPIOx, uint16_t GPIO_Pin)
{	 
    uint8_t bitstatus = 0x00;
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
    if (((GPIOx->PDIR >> GPIO_Pin)& 0x01) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;
    }
    return bitstatus;
}
/***********************************************************************************************
 功能：读取一个IO端口的输入电平
 形参：GPIOx:
			 @arg PTA:A端口
			 @arg PTB:B端口
			 @arg PTC:C端口
			 @arg PTD:D端口
			 @arg PTE:E端口
 返回：端口电平
 详解：必须先设置端口为输入端口
************************************************************************************************/
uint32_t GPIO_ReadInputData(GPIO_Type *GPIOx)
{
    //检测参数
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
    return(GPIOx->PDIR);
}

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
void GPIO_DMACmd(GPIO_Type* GPIOx, GPIO_DMASelect_TypeDef GPIO_DMAReq, GPIO_Pin_Type GPIO_Pin, FunctionalState NewState)
{
    PORT_Type *PORTx = NULL;
    PORTx->PCR[(uint8_t)GPIO_Pin] &= ~PORT_PCR_IRQC_MASK;
    switch((uint32_t)GPIOx)
    {
        case PTA_BASE:
            PORTx = PORTA;
            break; 
        case PTB_BASE:
            PORTx = PORTB;
            break;	
        case PTC_BASE:
            PORTx = PORTC;
            break;	
        case PTD_BASE:
            PORTx = PORTD;
            break;	
        case PTE_BASE:
            PORTx = PORTE;
            break;	
        default: 
            break;
    } 
    PORTx->PCR[(uint8_t)GPIO_Pin] &= ~PORT_PCR_IRQC_MASK;
    (ENABLE == NewState)?(PORTx->PCR[(uint8_t)GPIO_Pin] |= PORT_PCR_IRQC((uint8_t)GPIO_DMAReq)):(PORTx->PCR[(uint8_t)GPIO_Pin] &= ~PORT_PCR_IRQC(0x00));
}

//! @}

//! @}

//! @}
