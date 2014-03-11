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


//!< Leagacy Support for Kineis Z Version(Inital Version)
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

//!< Gloabl Const Table Defination
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
static const IRQn_Type GPIO_IRQnTable[] = 
{
    PORTA_IRQn,
    PORTB_IRQn,
    PORTC_IRQn,
    PORTD_IRQn,
    PORTE_IRQn,
};

//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{

 /**
 * @brief  设置引脚复用功能 这个函数会被很多其他外设模块驱动程序调用
 * 
 * @code
 *      // 将一个IO口复用功能设置为Mux1
 *      PORT_PinMuxConfig(HW_GPIOA, 3, kPinAlt1);
 * @endcode
 * @param  instance: GPIO模块号 可选如下
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 引脚号0-31
 * @param  pinMux:   复用功能选项 不同的复用值代表不同的功能 比如 kPinAlt1 一般代表用做GPIO
 *         @arg kPinAlt0
 *         @arg ...
 *         @arg kPinAlt7
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
 * @brief  设置一个引脚的属性 例如上拉下拉 复用选择 等 用户一般不必调用
 *
 * @code
 *      // 将一个引脚设置为 配有上啦电阻 开漏输出开启
 *      PORT_PinMuxConfig(HW_GPIOA, 3, kPullUp, ENABLE);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31 引脚
 * @param  pull: 模式选择
 *         @arg kPullDisabled:禁止上下拉
 *         @arg kPullUp: 上拉
 *         @arg kPullDown:下拉
 * @param  newState: 是否开启开漏输出模式 这个选项只在Kinetis K系列上有效果.
 * @retval None
 */
void PORT_PinPullConfig(uint8_t instance, uint8_t pinIndex, PORT_Pull_Type pull)
{
    //param check
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

void PORT_PinOpenDrainConfig(uint8_t instance, uint8_t pinIndex, FunctionalState newState)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (newState == ENABLE) ? (PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_ODE_MASK);
}

 /**
 * @brief  config 设置引脚为输入还是输出 只有当引脚作为GPIO时候才有意义
 * @param  instance: GPIO 模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31引脚号
 * @param  mode: 输入或者输出
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
 * @brief  通过填入初始化结构体 初始化GPIO
 *
 * @code
 *       //初始化引脚作为GPIO功能(PTB10) 推挽输出 并翻转该引脚的电平
 *  GPIO_InitTypeDef GPIO_InitStruct1;
 *  GPIO_InitStruct1.instance = HW_GPIOB;
 *  GPIO_InitStruct1.mode = kGPIO_Mode_OPP; //推挽输出
 *  GPIO_InitStruct1.pinx = 10;
 *  //初始化GPIO 
 *  GPIO_Init(&GPIO_InitStruct1);
 *  while(1)
 *  {
 *      DelayMs(200); //延时200m
 *      //翻转该引脚电平
 *      GPIO_ToggleBit(HW_GPIOB, 10);
 *  }
 * @endcode
 * @param  GPIO_InitStruct: GPIO初始化结构
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
    //config pinMux
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

 /**
 * @brief  快速初始化一个GPIO引脚 实际上是GPIO_Init的最简单配置
 * @code
 *      // 初始化一个GPIO口为输出 并初始化为高电平
 *      GPIO_QuickInit(HW_GPIOA, 3, kGPIO_Mode_OPP);
 *      GPIO_WriteBit(HW_GPIOA, 3, 1);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @param  mode: pin type
 *         @arg kGPIO_Mode_IFT:悬空输入
 *         @arg kGPIO_Mode_IPD:下拉输入
 *         @arg kGPIO_Mode_IPU:上啦输入
 *         @arg kGPIO_Mode_OOD:开漏输出
 *         @arg kGPIO_Mode_OPP:推挽输出
 * @retval None
 */
uint8_t GPIO_QuickInit(uint8_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    GPIO_InitStruct1.instance = instance;
    GPIO_InitStruct1.mode = mode;
    GPIO_InitStruct1.pinx = pinx;
    GPIO_Init(&GPIO_InitStruct1);
    return  instance;
}

 /**
 * @brief  输出高电平或者低电平(假设此引脚为输出功能)
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @param  data:
 *         @arg 0 : 低电平 
 *         @arg 1 : 高电平
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
 * @brief  读取一个引脚上的电平状态
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @retval 
 *         @arg 0 : 低电平
 *         @arg 1 : 高电平
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
 * @brief  向一个GPIO端口写入32位数据
 * @param  instance: GPIO 模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param data: 32位数据
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
 * @brief  设置GPIO引脚中断 或者 引脚DMA功能
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param config: 配置模式
 * @param newState: 使能或者禁止
 * @retval None
 */
void GPIO_ITDMAConfig(uint8_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    
    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
    switch(config)
    {
        case kGPIO_IT_Disable:
            NVIC_DisableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_DMA_Disable:
            break;
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
 * @brief  注册中断回调函数
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param AppCBFun: 回调函数指针
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

//! @}



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
