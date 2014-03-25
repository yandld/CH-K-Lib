/**
  ******************************************************************************
  * @file    pit.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
	* @note    此文件为芯片PIT模块的底层功能函数
  ******************************************************************************
  */
#include "pit.h"
#include "common.h"
#include "clock.h"

//!< Internal vars
static uint32_t fac_us; //!< usDelay Mut
static uint32_t fac_ms; //!< msDelay Mut

static PIT_CallBackType PIT_CallBackTable[4] = {NULL};
static const IRQn_Type PIT_IRQnTable[] = 
{
    PIT0_IRQn,
    PIT1_IRQn,
    PIT2_IRQn,
    PIT3_IRQn,
};

//! @defgroup CHKinetis
//! @{


//! @defgroup PIT
//! @brief PIT API functions
//! @{

 /**
 * @brief  详细初始化PIT模块 推荐使用PIT_QuickInit函数
 * @code
 *      // 配置PIT0模块的0通道，时间周期为1ms
 *      PIT_InitTypeDef PIT_InitStruct1; //申请一个结构变量
 *      PIT_InitStruct1.chl = 0;         //选择0通道
 *      PIT_InitStruct1.timeInUs = 1000  //1ms
 *      PIT_Init(&PIT_InitStruct1);
 * @endcode
 * @param  PIT_InitStruct: pit模块工作配置数据
 * @retval None
 */
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    //get clock
    CLOCK_GetClockFrequency(kBusClock, &fac_us);
    fac_us /= 1000000;
    fac_ms = (fac_us * 1000);
    fac_ms = fac_ms;
    PIT->CHANNEL[PIT_InitStruct->chl].LDVAL = fac_us * PIT_InitStruct->timeInUs;
    PIT_StartCounting(PIT_InitStruct->chl);
    PIT_Cmd(ENABLE);
}

 /**
 * @brief  PIT模块快速初始化配置
 * @code
 *      // 初始化PIT模块 0 通道 产生100MS中断 并开启中断 注册回调函数 在回调函数中打印调试信息
 *      //声明中断回调函数
 *      static void PIT0_CallBack(void);
 *      //初始化PIT模块的0通道，产生100ms中断
 *      PIT_QuickInit(HW_PIT_CH0, 100000);
 *      PIT_CallbackInstall(HW_PIT0_CH0, PIT0_CallBack); //注册回调函数
 *      PIT_ITDMAConfig(HW_PIT0_CH0, ENABLE);            //开启中断
 *      //中断回调函数
 *      static void PIT0_CallBack(void)
 *      {
 *          printf("Enter PIT0 INt\r\n");    
 *      }
 * @endcode
 * @param  chl 通道号
 *         @arg HW_PIT0_CH0 
 *         @arg HW_PIT0_CH1
 *         @arg HW_PIT0_CH2 
 *         @arg HW_PIT0_CH3 
 * @param  timeInUs 产生中断的周期 \单位US
 * @retval None
 */
void PIT_QuickInit(uint8_t chl, uint32_t timeInUs)
{
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.chl = chl;
    PIT_InitStruct1.timeInUs = timeInUs;
    PIT_Init(&PIT_InitStruct1);
    PIT_StartCounting(chl);
}

 /**
 * @brief  设置PIT模块是否开启中断功能
 * 
 * @code
 *      // 初始化PIT模块 0 通道 产生100MS中断 并开启中断 注册回调函数 在回调函数中打印调试信息
 *      //声明中断回调函数
 *      static void PIT0_CallBack(void);
 *      //初始化PIT
 *      PIT_QuickInit(HW_PIT_CH0, 100000);
 *      PIT_CallbackInstall(HW_PIT0_CH0, PIT0_CallBack); //注册回调函数
 *      PIT_ITDMAConfig(HW_PIT0_CH0, ENABLE);            //开启0通道中断
 *      //中断回调函数编写
 *      static void PIT0_CallBack(void)
 *      {
 *          printf("Enter PIT0 INt\r\n");    
 *      }
 * @endcode
 * @param  chl  :通道号
 *         @arg HW_PIT0_CH0   :0通道
 *         @arg HW_PIT0_CH1   :1通道
 *         @arg HW_PIT0_CH2   :2通道
 *         @arg HW_PIT0_CH3   :3通道
 * @param  NewState ENABLE 或DISABLE
 * @retval None
 */
void PIT_ITDMAConfig(uint8_t chl, PIT_ITDMAConfig_Type config)
{
    switch (config)
    {
        case kPIT_IT_Disable:
            //NVIC_DisableIRQ(PIT_IRQnTable[chl]);
            PIT->CHANNEL[chl].TCTRL &= ~PIT_TCTRL_TIE_MASK;
            break;
        case kPIT_IT_TOF:
            NVIC_EnableIRQ(PIT_IRQnTable[chl]);
            PIT->CHANNEL[chl].TCTRL |= PIT_TCTRL_TIE_MASK;
            break;
        default:
            break;
    }
}

 /**
 * @brief  PIT通道定时器开始计数
 * @code
 *      // 开始PIT模块 0 通道的计数
 *      PIT_StartCounting(HW_PIT0_CH0);
 * @endcode
 * @param  chl  :通道号
 *         @arg HW_PIT0_CH0   :0通道
 *         @arg HW_PIT0_CH1   :1通道
 *         @arg HW_PIT0_CH2   :2通道
 *         @arg HW_PIT0_CH3   :3通道
 * @retval None
 */
void PIT_StartCounting(uint8_t chl)
{
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

 /**
 * @brief  PIT通道定时器停止计数
 * @code
 *      //停止PIT模块 0 通道的计数
 *      PIT_StopCounting(HW_PIT0_CH0);
 * @endcode
 * @param  chl  :通道号
 *         @arg HW_PIT0_CH0   :0通道
 *         @arg HW_PIT0_CH1   :1通道
 *         @arg HW_PIT0_CH2   :2通道
 *         @arg HW_PIT0_CH3   :3通道
 * @retval None
 */
void PIT_StopCounting(uint8_t chl)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
}

 /**
 * @brief  开启或关闭PIT模块 在初始化后默认开启
 * @code
 *      //关闭PIT模块
 *      PIT_Cmd(DISABLE);
 * @endcode
 * @param  NewState
 *         @arg ENABLE  : 开启
 *         @arg DISABLE : 关闭
 * @retval None
 */
void PIT_Cmd(FunctionalState NewState)
{
    (ENABLE == NewState)?(PIT->MCR &= ~PIT_MCR_MDIS_MASK):(PIT->MCR |= PIT_MCR_MDIS_MASK);
}

//! @}

//! @}
/**
 * @brief  注册中断回调函数
 * @param  chl  :通道号
 *         @arg HW_PIT0_CH0   :0通道入口
 *         @arg HW_PIT0_CH1   :1通道入口
 *         @arg HW_PIT0_CH2   :2通道入口
 *         @arg HW_PIT0_CH3   :3通道入口
 * @param AppCBFun: 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void PIT_CallbackInstall(uint8_t chl, PIT_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        PIT_CallBackTable[chl] = AppCBFun;
    }
}


void PIT0_IRQHandler(void)
{
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
    if(PIT_CallBackTable[0])
    {
        PIT_CallBackTable[0]();
    }
}

void PIT1_IRQHandler(void)
{
    PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
    if(PIT_CallBackTable[1])
    {
        PIT_CallBackTable[1]();
    }
}

void PIT2_IRQHandler(void)
{
    PIT->CHANNEL[2].TFLG |= PIT_TFLG_TIF_MASK;
    if(PIT_CallBackTable[2])
    {
        PIT_CallBackTable[2]();
    }
}

void PIT3_IRQHandler(void)
{
    PIT->CHANNEL[3].TFLG |= PIT_TFLG_TIF_MASK;
    if(PIT_CallBackTable[3])
    {
        PIT_CallBackTable[3]();
    }
}

