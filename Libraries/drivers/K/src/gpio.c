/**
  ******************************************************************************
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
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


//!< Leagacy Support for Kineis Z Version(Inital Version)
#if (!defined(GPIO_BASES))

    #if (defined(MK60DZ10))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #endif

#endif

//!< Gloabl Const Table Defination
static IRQn_Type   const GPIO_IRQBase = PORTA_IRQn;
static GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};
static const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK,
};

	/**
  * @brief  Config PinMux function. This function is used in many other drivers code
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @param  pinMux:   0 - kPinAltNameCount different pinmux value indcatie different pin function. It's various form chip to chip.
  * @retval None
  */
void PORT_PinMuxConfig(uint8_t instance, uint8_t pinIndex, PORT_PinMux_Type pinMux)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
		PORT_InstanceTable[instance]->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);
		PORT_InstanceTable[instance]->PCR[pinIndex] |=  PORT_PCR_MUX(pinMux);
}
	/**
  * @brief  Config pin pull select and open drain enablement
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @param  pull: pull select
  *         @arg kPullDisabled
  *         @arg kPullUp
  *         @arg kPullDown
  * @param  newState: enable open drain or not. This select only vailid on K series.
  * @retval None
  */
void PORT_PinConfig(uint8_t instance, uint8_t pinIndex, PORT_Pull_Type pull, FunctionalState newState)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
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
            break;
		}
}
	/**
  * @brief  config GPIO pin input or output
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @param  mode: input or output
  *         @arg kInpput
  *         @arg kOutput
  * @retval None
  */
void GPIO_PinConfig(uint8_t instance, uint8_t pinIndex, GPIO_PinConfig_Type mode)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (mode == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pinIndex));
}

	/**
  * @brief  Initializes the GPIOx peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(GPIO_InitStruct->instance));
		assert_param(IS_PORT_ALL_INSTANCE(GPIO_InitStruct->instance));
		assert_param(IS_GPIO_ALL_PIN(GPIO_InitStruct->pinx));
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
	/**
  * @brief  Quick init for user, do not need init struct
  * @code
  *      // init PA3 a pin as output
  *      GPIO_QuickInit(HW_GPIOA, 3, kGPIO_Mode_OPP);
  * @endcode
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @param  mode: pin type
  *         @arg kGPIO_Mode_IFT
  *         @arg kGPIO_Mode_IPD
  *         @arg kGPIO_Mode_IPU
  *         @arg kGPIO_Mode_OOD
  *         @arg kGPIO_Mode_OPP
  * @retval None
  */
void GPIO_QuickInit(uint8_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
		GPIO_InitStruct1.instance = instance;
		GPIO_InitStruct1.mode = mode;
		GPIO_InitStruct1.pinx = pinx;
		GPIO_Init(&GPIO_InitStruct1);
}

	/**
  * @brief  Write a data bit to GPIO
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @param  data:
	*         @arg 0 : low state
	*         @arg 1 : high state
  * @retval None
  */
void GPIO_WriteBit(uint8_t instance, uint8_t pinIndex, uint8_t data)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pinIndex));
}
	/**
  * @brief  read a data bit from GPIO
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @retval 
	*         @arg 0 : low state
	*         @arg 1 : high state
  */
uint8_t GPIO_ReadBit(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    //!< input or output
    if(((GPIO_InstanceTable[instance]->PDDR) >> pinIndex) & 0x01)
		{
        return ((GPIO_InstanceTable[instance]->PDOR >> pinIndex) & 0x01);
		}
		else
		{
        return ((GPIO_InstanceTable[instance]->PDIR >> pinIndex) & 0x01);
		}
}
	/**
  * @brief  Toggle a GPIO single bit
  * @param  instance: GPIO instance
  *         @arg HW_GPIOA
  *         @arg HW_GPIOB
  *         @arg HW_GPIOC
  *         @arg HW_GPIOD
	*         @arg HW_GPIOE
  * @param  pinIndex: 0-31
  * @retval None
  */
void GPIO_ToggleBit(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PTOR |= (1 << pinIndex);
}


/**
 * @brief  Read 32 bit data form whole GPIO port
 * @param  instance: GPIO instance
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @retval prot data(32 bit)
 */
uint32_t GPIO_ReadByte(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    return (GPIO_InstanceTable[instance]->PDIR);
}
/**
 * @brief  write 32 bit data form whole GPIO port
 * @param  instance: GPIO instance
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param data: 32bit data to write
 * @retval None
 */
void GPIO_WriteByte(uint8_t instance, uint8_t pinIndex, uint32_t data)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
		assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PDOR = data;
}

/**
 * @brief  GPIO dma and interrupt config
 * @param  instance: GPIO instance
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param config: dma and interrupt config mode
 * @param newState: enable interrupt or disable it
 * @retval None
 */
void GPIO_ITDMAConfig(uint8_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, FunctionalState newState)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    // disable interrupt first
    NVIC_DisableIRQ((IRQn_Type)(GPIO_IRQBase + instance));
    PORT_InstanceTable[instance]->PCR[(uint8_t)pinIndex] &= ~PORT_PCR_IRQC_MASK;
    //config
    PORT_InstanceTable[instance]->PCR[(uint8_t)pinIndex] |= PORT_PCR_IRQC(config);
    //enable interrupt
    (ENABLE == newState)?(NVIC_EnableIRQ((IRQn_Type)(GPIO_IRQBase + instance))):(NVIC_DisableIRQ((IRQn_Type)(GPIO_IRQBase + instance)));
}
/**
 * @brief  install interrupt callback function
 * @param  instance: GPIO instance
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param AppCBFun: callback function pointer
 * @retval None
 */
void GPIO_CallbackInstall(uint8_t instance, GPIO_CallBackType AppCBFun)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
		assert_param(IS_PORT_ALL_INSTANCE(instance));
    if(AppCBFun != NULL)
		{
        GPIO_CallBackTable[instance] = AppCBFun;
		}
}

void PORTA_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOA]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOA]->ISFR = 0xFFFFFFFF;
		if(GPIO_CallBackTable[HW_GPIOA])
		{
        GPIO_CallBackTable[HW_GPIOA](ISFR);
		}	
}

void PORTB_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOB]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOB]->ISFR = 0xFFFFFFFF;
		if(GPIO_CallBackTable[HW_GPIOB])
		{
        GPIO_CallBackTable[HW_GPIOB](ISFR);
		}	
}

void PORTC_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOC]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOC]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOC])
		{
        GPIO_CallBackTable[HW_GPIOC](ISFR);
		}	
}

void PORTD_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOD]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOD]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOD])
		{
        GPIO_CallBackTable[HW_GPIOD](ISFR);
		}	
}

void PORTE_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOE]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOE]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOE])
		{
        GPIO_CallBackTable[HW_GPIOE](ISFR);
		}	
}

#if (defined(MK70F12))
void PORTF_IRQHandler(void)
{
    uint32_t ISFR;
		//safe copy
    ISFR = PORT_InstanceTable[HW_GPIOF]->ISFR;
		//clear IT pending bit
		PORT_InstanceTable[HW_GPIOF]->ISFR = PORT_ISFR_ISF_MASK;
		if(GPIO_CallBackTable[HW_GPIOF])
		{
        GPIO_CallBackTable[HW_GPIOF](ISFR);
		}
}
#endif


//! @}

//! @}

//! @}
