#include "common.h"
#include "gpio.h"

/* gloabl vars */
static GPIO_Type * const GPIOCLKTbl[] = GPIO_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(GPIOCLKTbl)] = {NULL};

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTA_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTB_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTC_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTD_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTE_MASK, SIM_SCGC5_PORTE_SHIFT},
};

static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)(PORTA_IRQn+0),
    (IRQn_Type)(PORTA_IRQn+1),
    (IRQn_Type)(PORTA_IRQn+2),
    (IRQn_Type)(PORTA_IRQn+3),
    (IRQn_Type)(PORTA_IRQn+4),
};



//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{


 /**
 * @brief  设置引脚为输入还是输出功能  用户一般不必调用
 * @note   只有当引脚作为GPIO时才有意义
 * @code
 *      // 将PORTB端口的3引脚设置输入引脚
 *      GPIO_PinConfig(HW_GPIOB, 3, kInpput);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @param  mode      :输入或者输出设置
 *         @arg kInpput  :输入功能选择
 *         @arg kOutput  :输出功能选择
 * @retval None
 */
void GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir)
{
    CLK_EN(CLKTbl, instance);
    (dir == 1) ? (GPIOCLKTbl[instance]->PDDR |= (1 << pin)):
    (GPIOCLKTbl[instance]->PDDR &= ~(1 << pin));
}

 /**
 * @brief  GPIO初始化配置
 * @code
 *    //初始化配置PORTB端口的10引脚为推挽输出引脚
 *    GPIO_InitTypeDef GPIO_InitStruct1;      //申请一个结构变量
 *    GPIO_InitStruct1.instance = HW_GPIOB;   //选择PORTB端口
 *    GPIO_InitStruct1.mode = kGPIO_Mode_OPP; //推挽输出
 *    GPIO_InitStruct1.pinx = 10;             //选择10引脚
 *    //调用初始化GPIO函数 
 *    GPIO_Init(&GPIO_InitStruct1);
 * @endcode
 * @param  GPIO_InitStruct: GPIO初始化结构体，包含了引脚状态参数  
           GPIO_InitStruct.instance   :端口号 HW_GPIOA ~ HW_GPIOE
 * @retval None
 */
void GPIO_Init(GPIO_InitTypeDef * Init)
{
    /* config state */
    switch(Init->mode)
    {
        case kGPIO_Mode_IFT:
            SetPinPull(Init->instance, Init->pinx, 0xFF);
            GPIO_SetPinDir(Init->instance, Init->pinx, 0);
            break;
        case kGPIO_Mode_IPD:
            SetPinPull(Init->instance, Init->pinx, 0);
            GPIO_SetPinDir(Init->instance, Init->pinx, 0);
            break;
        case kGPIO_Mode_IPU:
            SetPinPull(Init->instance, Init->pinx, 1);
            GPIO_SetPinDir(Init->instance, Init->pinx, 0);
            break;
        case kGPIO_Mode_OPP:
            SetPinPull(Init->instance, Init->pinx, 0xFF);
            GPIO_SetPinDir(Init->instance, Init->pinx, 1);
            break;
        default:
            break;					
    }
    /* config pinMux */
    SetPinMux(Init->instance, Init->pinx, kPinAlt1);
}

 /**
 * @brief  快速初始化一个GPIO引脚 实际上是GPIO_Init的最简单配置
 * @code
 *      //初始化配置PORTB端口的10引脚为推挽输出引脚
 *      GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @param  mode  :引脚工作模式
 *         @arg kGPIO_Mode_IFT :悬空输入
 *         @arg kGPIO_Mode_IPD :下拉输入
 *         @arg kGPIO_Mode_IPU :上拉输入
 *         @arg kGPIO_Mode_OOD :开漏输出
 *         @arg kGPIO_Mode_OPP :推挽输出
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
 * @brief  设置指定引脚输出高电平或者低电平
 * @note   此引脚首先配置成输出引脚
 * @code
 *      //设置PORTB端口的10引脚输出高电平
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @param  data   : 引脚的电平状态  
 *         @arg 0 : 低电平 
 *         @arg 1 : 高电平
 * @retval None
 */
void GPIO_WriteBit(uint32_t instance, uint8_t pinIndex, uint8_t data)
{
    (data) ? (GPIOCLKTbl[instance]->PSOR |= (1 << pinIndex)):(GPIOCLKTbl[instance]->PCOR |= (1 << pinIndex));
}
 /**
 * @brief  读取一个引脚上的电平状态
 * @code
 *      //读取PORTB端口的10引脚的电平状态
 *      uint8_t status ; //用于存储引脚的状态
 *      status = GPIO_ReadBit(HW_GPIOB, 10); //获取引脚的状态并存储到status中
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @retval 
 *         @arg 0 : 低电平
 *         @arg 1 : 高电平
 */
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pinIndex)
{
    /* input or output */
    if(((GPIOCLKTbl[instance]->PDDR) >> pinIndex) & 0x01)
    {
        return ((GPIOCLKTbl[instance]->PDOR >> pinIndex) & 0x01);
    }
    else
    {
        return ((GPIOCLKTbl[instance]->PDIR >> pinIndex) & 0x01);
    }
}

 /**
 * @brief  翻转一个引脚的电平状态
 * @code
 *      //翻转PORTB端口的10引脚的电平状态
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @retval None
 */
void GPIO_ToggleBit(uint32_t instance, uint8_t pinIndex)
{
    GPIOCLKTbl[instance]->PTOR |= (1 << pinIndex);
}

/**
 * @brief  读取一个端口32位的数据
 * @code
 *      //获取PORTB端口的所有引脚的电平状态
 *      uint32_t status ; //用于存储引脚的状态
 *      status = GPIO_ReadPort(HW_GPIOB); //获取引脚的状态并存储到status中
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @retval 端口的32位数据
 */
uint32_t GPIO_Get(uint32_t instance)
{
    return (GPIOCLKTbl[instance]->PDIR);
}
/**
 * @brief  向一个端口写入32位数据
 * @code
 *      //向PORTB端口写入0xFFFFFFFF
 *      GPIO_WriteByte(HW_GPIOB, 0xFFFFFFFF); 
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  data  :32位数据
 * @retval None
 */
void GPIO_Send(uint32_t instance, uint32_t data)
{
    GPIOCLKTbl[instance]->PDOR = data;
}

/**
 * @brief  设置GPIO引脚中断类型或者DMA功能
 * @code
 *      //设置PORTB端口的10引脚为下降沿触发中断
 *      GPIO_ITDMAConfig(HW_GPIOB, 10, kGPIO_IT_FallingEdge, true); 
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @param config: 配置模式
 *         @arg kGPIO_DMA_RisingEdge DMA上升沿触发
 *         @arg kGPIO_DMA_FallingEdge DMA下降沿触发
 *         @arg kGPIO_DMA_RisingFallingEdge DMA上升和下降沿都触发
 *         @arg kGPIO_IT_Low 低电平触发中断
 *         @arg kGPIO_IT_RisingEdge 上升沿触发中断
 *         @arg kGPIO_IT_FallingEdge 下降沿触发中断
 *         @arg kGPIO_IT_RisingFallingEdge 上升和下降沿都触发中断
 *         @arg kGPIO_IT_High 高电平触发中断
 * @retval None
 */
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, bool status)
{
    CLK_EN(CLKTbl, instance);
//    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
    
    if(!status)
    {
        NVIC_DisableIRQ(GPIO_IrqTbl[instance]);
        return;
    }
    
    switch(config)
    {
        case kGPIO_DMA_RisingEdge:
           // PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FallingEdge:
          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_RisingFallingEdge:
          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(3);
            break;
        case kGPIO_IT_Low:
          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(8);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_RisingEdge:
         //   PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(9);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_FallingEdge:
        //    PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(10);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_RisingFallingEdge:
         //   PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(11);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_High:
        //    PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(12);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        default:
            break;
    }
}
/**
 * @brief  注册中断回调函数
 * @param  instance: GPIO模块中断入口号
 *         @arg HW_GPIOA :芯片的PORTA端口中断入口
 *         @arg HW_GPIOB :芯片的PORTB端口中断入口
 *         @arg HW_GPIOC :芯片的PORTC端口中断入口
 *         @arg HW_GPIOD :芯片的PORTD端口中断入口
 *         @arg HW_GPIOE :芯片的PORTE端口中断入口
 * @param AppCBFun: 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun)
{
    CLK_EN(CLKTbl, instance);
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[instance] = AppCBFun;
    }
}

//! @}


void PORTA_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
   // ISFR = PORT_InstanceTable[HW_GPIOA]->ISFR;
    /* clear IT pending bit */
  //  PORT_Inst/ceTable[HW_GPIOA]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[HW_GPIOA])
    {
        GPIO_CallBackTable[HW_GPIOA](ISFR);
    }	
}




//! @}
