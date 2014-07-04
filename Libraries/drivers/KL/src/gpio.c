/**
  ******************************************************************************
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ??????GPIO?????????
  ******************************************************************************
  */

#include "gpio.h"

/* leagacy support for Kineis Z version(inital version) */
#if (!defined(GPIO_BASES))

    #if (defined(MK60DZ10))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #endif
    #if (defined(MK10D5))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #endif
#endif

/* gloabl vars */
static GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};
static const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
};
static const IRQn_Type GPIO_IRQnTable[] = 
{
    PORTA_IRQn,
    PORTB_IRQn,
};

//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{

 /**
 * @brief ???????? ????????????????????
 * @note  ??????? Reference Manual ? Signal Multiplexing and Signal Descriptions ?? 
 * @code
 *      // ??PORTA???3?????1??.
 *      PORT_PinMuxConfig(HW_GPIOA, 3, kPinAlt1);
 * @endcode
 * @param  instance: GPIO??? 
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  pinMux    :??????,?????????????
 *         @arg kPinAlt0 :?????0??
 *         @arg        . : .
 *         @arg        . : .
 *         @arg        . : .
 *         @arg kPinAlt7 :?????7??
 * @retval None
 */
void PORT_PinMuxConfig(uint32_t instance, uint8_t pinIndex, PORT_PinMux_Type pinMux)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);
    PORT_InstanceTable[instance]->PCR[pinIndex] |=  PORT_PCR_MUX(pinMux);
}
 /**
 * @brief  ?????????????? ????????
 * @code
 *      // ?PORTA???3?????????(???????)
 *      PORT_PinPullConfig(HW_GPIOA, 3, kPullUp);
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  pull      :????????
 *         @arg kPullDisabled :???????
 *         @arg kPullUp       :????
 *         @arg kPullDown     :????
 * @retval None
 */
void PORT_PinPullConfig(uint32_t instance, uint8_t pinIndex, PORT_Pull_Type pull)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
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
 * @brief  ??????????? ????????
 * @code
 *      // ?PORTA???3????????? 
 *      PORT_PinOpenDrainConfig(HW_GPIOA, 3, ENABLE);
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  newState  :??????
 *         @arg ENABLE   :????
 *         @arg DISABLE  :????
 * @retval None
 */
void PORT_PinOpenDrainConfig(uint32_t instance, uint8_t pinIndex, FunctionalState newState)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
//    (newState == ENABLE) ? (PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_ODE_MASK);
}

/**
 * @brief  ???????????? ???????
 * @code
 *      // ?PORTA???3????????? 
 *      PORT_PinPassiveFilterConfig(HW_GPIOA, 3, ENABLE);
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  newState  :??????
 *         @arg ENABLE   :????
 *         @arg DISABLE  :????
 * @retval None
 */
void PORT_PinPassiveFilterConfig(uint32_t instance, uint8_t pinIndex, FunctionalState newState)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (newState == ENABLE) ? (PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PFE_MASK):(PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PFE_MASK);
}

 /**
 * @brief  ?????????????  ????????
 * @note   ???????GPIO?????
 * @code
 *      // ?PORTB???3????????
 *      GPIO_PinConfig(HW_GPIOB, 3, kInpput);
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  mode      :????????
 *         @arg kInpput  :??????
 *         @arg kOutput  :??????
 * @retval None
 */
void GPIO_PinConfig(uint32_t instance, uint8_t pinIndex, GPIO_PinConfig_Type mode)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (mode == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pinIndex));
}

 /**
 * @brief  GPIO?????
 * @code
 *    //?????PORTB???10?????????
 *    GPIO_InitTypeDef GPIO_InitStruct1;      //????????
 *    GPIO_InitStruct1.instance = HW_GPIOB;   //??PORTB??
 *    GPIO_InitStruct1.mode = kGPIO_Mode_OPP; //????
 *    GPIO_InitStruct1.pinx = 10;             //??10??
 *    //?????GPIO?? 
 *    GPIO_Init(&GPIO_InitStruct1);
 * @endcode
 * @param  GPIO_InitStruct: GPIO??????,?????????  
           GPIO_InitStruct.instance   :??? HW_GPIOA ~ HW_GPIOE
 * @retval None
 */
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(GPIO_InitStruct->instance));
    assert_param(IS_PORT_ALL_INSTANCE(GPIO_InitStruct->instance));
    assert_param(IS_GPIO_ALL_PIN(GPIO_InitStruct->pinx));
    /* config state */
    switch(GPIO_InitStruct->mode)
    {
        case kGPIO_Mode_IFT:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDown);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPU:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_OOD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, ENABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        case kGPIO_Mode_OPP:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        default:
            break;					
    }
    /* config pinMux */
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

 /**
 * @brief  ???????GPIO?? ????GPIO_Init??????
 * @code
 *      //?????PORTB???10?????????
 *      GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  mode  :??????
 *         @arg kGPIO_Mode_IFT :????
 *         @arg kGPIO_Mode_IPD :????
 *         @arg kGPIO_Mode_IPU :????
 *         @arg kGPIO_Mode_OOD :????
 *         @arg kGPIO_Mode_OPP :????
 * @retval None
 */
uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    GPIO_InitStruct1.instance = instance;
    GPIO_InitStruct1.mode = mode;
    GPIO_InitStruct1.pinx = pinx;
    GPIO_Init(&GPIO_InitStruct1);
    return  instance;
}

 /**
 * @brief  ????????????????
 * @note   ????????????
 * @code
 *      //??PORTB???10???????
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param  data   : ???????  
 *         @arg 0 : ??? 
 *         @arg 1 : ???
 * @retval None
 */
void GPIO_WriteBit(uint32_t instance, uint8_t pinIndex, uint8_t data)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pinIndex));
}
 /**
 * @brief  ????????????
 * @code
 *      //??PORTB???10???????
 *      uint8_t status ; //?????????
 *      status = GPIO_ReadBit(HW_GPIOB, 10); //???????????status?
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @retval 
 *         @arg 0 : ???
 *         @arg 1 : ???
 */
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pinIndex)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    /* input or output */
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
 * @brief  ???????????
 * @code
 *      //??PORTB???10???????
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @retval None
 */
void GPIO_ToggleBit(uint32_t instance, uint8_t pinIndex)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PTOR |= (1 << pinIndex);
}

/**
 * @brief  ??????32????
 * @code
 *      //??PORTB????????????
 *      uint32_t status ; //?????????
 *      status = GPIO_ReadPort(HW_GPIOB); //???????????status?
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @retval ???32???
 */
uint32_t GPIO_ReadPort(uint32_t instance)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    return (GPIO_InstanceTable[instance]->PDIR);
}
/**
 * @brief  ???????32???
 * @code
 *      //?PORTB????0xFFFFFFFF
 *      GPIO_WriteByte(HW_GPIOB, 0xFFFFFFFF); 
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  data  :32???
 * @retval None
 */
void GPIO_WritePort(uint32_t instance, uint32_t data)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    GPIO_InstanceTable[instance]->PDOR = data;
}

/**
 * @brief  ??GPIO????????DMA??
 * @code
 *      //??PORTB???10??????????
 *      GPIO_ITDMAConfig(HW_GPIOB, 10, kGPIO_IT_FallingEdge); 
 * @endcode
 * @param  instance: GPIO???
 *         @arg HW_GPIOA :???PORTA??
 *         @arg HW_GPIOB :???PORTB??
 *         @arg HW_GPIOC :???PORTC??
 *         @arg HW_GPIOD :???PORTD??
 *         @arg HW_GPIOE :???PORTE??
 * @param  pinIndex  :??????? 0~31
 * @param config: ????
 *         @arg kGPIO_IT_Disable ??????
 *         @arg kGPIO_DMA_Disable ??DMA??
 *         @arg kGPIO_DMA_RisingEdge DMA?????
 *         @arg kGPIO_DMA_FallingEdge DMA?????
 *         @arg kGPIO_DMA_RisingFallingEdge DMA?????????
 *         @arg kGPIO_IT_Low ???????
 *         @arg kGPIO_IT_RisingEdge ???????
 *         @arg kGPIO_IT_FallingEdge ???????
 *         @arg kGPIO_IT_RisingFallingEdge ???????????
 *         @arg kGPIO_IT_High ???????
 * @retval None
 */
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, bool status)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    /* init moudle */
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
    
    if(!status)
    {
        NVIC_DisableIRQ(GPIO_IRQnTable[instance]);
        return;
    }
    
    switch(config)
    {
        case kGPIO_DMA_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(3);
            break;
        case kGPIO_IT_Low:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(8);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(9);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(10);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(11);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_High:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(12);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        default:
            break;
    }
}
/**
 * @brief  ????????
 * @param  instance: GPIO???????
 *         @arg HW_GPIOA :???PORTA??????
 *         @arg HW_GPIOB :???PORTB??????
 *         @arg HW_GPIOC :???PORTC??????
 *         @arg HW_GPIOD :???PORTD??????
 *         @arg HW_GPIOE :???PORTE??????
 * @param AppCBFun: ????????
 * @retval None
 * @note ?????????????????
 */
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun)
{
    /* param check */
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    /* init moudle */
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[instance] = AppCBFun;
    }
}

//! @}


/**
 * @brief  ????????
 * @param  PORTA_IRQHandler :???PORTA????????
 *         PORTB_IRQHandler :???PORTB????????
 *         PORTC_IRQHandler :???PORTC????????
 *         PORTD_IRQHandler :???PORTD????????
 *         PORTE_IRQHandler :???PORTE????????
 * @note ????????????
 */
void PORTA_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
    ISFR = PORT_InstanceTable[HW_GPIOA]->ISFR;
    /* clear IT pending bit */
    PORT_InstanceTable[HW_GPIOA]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[HW_GPIOA])
    {
        GPIO_CallBackTable[HW_GPIOA](ISFR);
    }	
}

void PORTB_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
    ISFR = PORT_InstanceTable[HW_GPIOB]->ISFR;
    /* clear IT pending bit */
    PORT_InstanceTable[HW_GPIOB]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[HW_GPIOB])
    {
        GPIO_CallBackTable[HW_GPIOB](ISFR);
    }	
}

void PORTC_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
    ISFR = PORT_InstanceTable[HW_GPIOC]->ISFR;
    /* clear IT pending bit */
    PORT_InstanceTable[HW_GPIOC]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOC])
    {
        GPIO_CallBackTable[HW_GPIOC](ISFR);
    }
}

void PORTD_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
    ISFR = PORT_InstanceTable[HW_GPIOD]->ISFR;
    /* clear IT pending bit */
    PORT_InstanceTable[HW_GPIOD]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOD])
    {
        GPIO_CallBackTable[HW_GPIOD](ISFR);
    }	
}

void PORTE_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
    ISFR = PORT_InstanceTable[HW_GPIOE]->ISFR;
    /* clear IT pending bit */
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
    /* safe copy */
    ISFR = PORT_InstanceTable[HW_GPIOF]->ISFR;
    /* clear IT pending bit */
    PORT_InstanceTable[HW_GPIOF]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOF])
    {
        GPIO_CallBackTable[HW_GPIOF](ISFR);
    }
}
#endif


//! @}
