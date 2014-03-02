/**
  ******************************************************************************
  * @file    pit.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "pit.h"
#include "common.h"
#include "clock.h"

//!< Internal vars
static uint32_t fac_us; //!< usDelay Mut
static uint32_t fac_ms; //!< msDelay Mut


#if (!defined(PIT_BASES))

#if     (defined(MK60DZ10))
#define PIT_BASES {PIT}
#elif   (defined(MK10D5))
#define PIT_BASES {PIT}
#endif

#endif


PIT_Type * const PIT_InstanceTable[] = PIT_BASES;
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
 * @brief  初始化PIT模块 推荐使用 PIT_QuickInit来进行快速初始化
 * 
 * @code
 *      // 初始化PIT模块
 *      PIT_InitTypeDef PIT_InitStruct1;
 *      PIT_InitStruct1.instance = 0;
 *      PIT_InitStruct1.chl = 0;
 *      PIT_InitStruct1.timeInUs = 1000 //1ms
 *      PIT_Init(&PIT_InitStruct1);
 * @endcode
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
    PIT_InstanceTable[PIT_InitStruct->instance]->CHANNEL[PIT_InitStruct->chl].LDVAL = fac_us * PIT_InitStruct->timeInUs;
    PIT_StartCounting(PIT_InitStruct->chl);
    PIT_Cmd(ENABLE);

}

 /**
 * @brief  初始化PIT模块 填入最重要的参数
 * 
 * @code
 *      // 初始化PIT模块 0 通道 产生100MS中断 并开启中断 注册回调函数 在回调函数中打印调试信息
 *      //声明中断回调函数
 *      static void PIT0_CallBack(void);
 *      //初始化PIT
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
    PIT_InitStruct1.instance = 0;
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
 * @param  NewState ENABLE 或DISABLE
 * @retval None
 */
void PIT_ITDMAConfig(uint8_t chl, FunctionalState NewState)
{
    if(ENABLE == NewState)
    {
        PIT->CHANNEL[chl].TFLG |= PIT_TFLG_TIF_MASK;
    }
    (ENABLE == NewState)?(PIT->CHANNEL[chl].TCTRL |= PIT_TCTRL_TIE_MASK):(PIT->CHANNEL[chl].TCTRL &= ~PIT_TCTRL_TIE_MASK);
    (ENABLE == NewState)?(NVIC_EnableIRQ(PIT_IRQnTable[chl])):(NVIC_DisableIRQ(PIT_IRQnTable[chl]));
}

 /**
 * @brief  PIT 定时器开始计数
 * 
 * @param  chl 通道号
 *         @arg HW_PIT0_CH0 
 *         @arg HW_PIT0_CH1
 *         @arg HW_PIT0_CH2 
 *         @arg HW_PIT0_CH3 
 * @retval None
 */
void PIT_StartCounting(uint8_t chl)
{
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

 /**
 * @brief  PIT 定时器停止计数
 * 
 * @param  chl 通道号
 *         @arg HW_PIT0_CH0 
 *         @arg HW_PIT0_CH1
 *         @arg HW_PIT0_CH2 
 *         @arg HW_PIT0_CH3 
 * @retval None
 */
void PIT_StopCounting(uint8_t chl)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
}

 /**
 * @brief  开启 或 关闭PIT模块 在初始化后默认开启
 * 
 * @param  NewState
 *         @arg ENABLE: 开启
 *         @arg ENABLE: 关闭
 * @retval None
 */
void PIT_Cmd(FunctionalState NewState)
{
    (ENABLE == NewState)?(PIT->MCR &= ~PIT_MCR_MDIS_MASK):(PIT->MCR |= PIT_MCR_MDIS_MASK);
}

//! @}

//! @}

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

