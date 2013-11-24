/**
  ******************************************************************************
  * @file    sys.h
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   超核K60固件库 系统级API函数头文件
  ******************************************************************************
  */
	 
#ifndef __SYS_H__
#define __SYS_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "common.h"
	 
//! @addtogroup CH_Periph_Driver
//! @{
	 
//! @addtogroup SYS
//! @{
	 
//! @addtogroup SYS_Exported_Types
//! @{


//! @brief Reset state code enum
typedef enum
{
	kSoftwareReset,                //!< Software reset
	kCoreLockupEventReset,         //!< Core lockup event reset
	kJTAGReset,                    //!< Jtag reset
	kPOReset,                      //!< Power on reset
	kExternalPinReset,             //!< External pin reset
	kWdogReset,                    //!< Wdog reset
	kLossOfClockReset,             //!< Loss of clock reset
	kLowVoltageDetectReset,        //!< Low voltage dectect reset
	kLLWUReset,                    //!< LLWU reset
}ResetStateCodeSelect_TpyeDef;



//! @brief [SystemClockUpdate] <clockSource> selection enumeration
typedef enum
{
	kClockSource_IRC,               //!< Interal ClockSource
  kClockSource_EX8M,              //!< External ClockSource 8MHZ
	kClockSource_EX50M,             //!< External ClockSource 50MHZ
}SYS_ClockSourceSelect_TypeDef; 
//! @brief  [SystemClockUpdate] <coreClock> selection enumeration
typedef enum
{
	kCoreClock_48M,                 //!< Coreclock = 48M
	kCoreClock_64M,                 //!< Coreclock = 64M
	kCoreClock_72M,                 //!< Coreclock = 72M
	kCoreClock_96M,                 //!< Coreclock = 96M
	kCoreClock_100M,                //!< Coreclock = 100M
	kCoreClock_200M,                //!< Coreclock = 200M
}SYS_CoreClockSelect_TypeDef;

//! @brief General peripheral Map define
typedef struct
{
	uint32_t m_ModuleIndex:3;
	uint32_t m_PortIndex:3;
	uint32_t m_Mux:3;
	uint32_t m_PinBase:5;
	uint32_t m_PinCnt:3;
	uint32_t m_Channel:5;
	uint32_t m_SpecDefine1:2;
	uint32_t m_SpecDefine2:2;
	uint32_t m_SpecDefine3:2;
	uint32_t m_SpecDefine4:2;
	uint32_t m_SpecDefine5:2;
}PeripheralMap_TypeDef;

//! @}

//! @addtogroup SYS_Constants_Macros
//! @{

#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority   4 bits for subpriority */                                               
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority   3 bits for subpriority */                                                  
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority   2 bits for subpriority */                                                   
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority   1 bits for subpriority */                                                   
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority   0 bits for subpriority */

//! @}



//! @addtogroup SYS_Exported_Variables
//! @{

//! @}

//参数检测器宏
#define IS_NVIC_PRIORITY_GROUP(GROUP) (((GROUP) == NVIC_PriorityGroup_0) || \
                                       ((GROUP) == NVIC_PriorityGroup_1) || \
                                       ((GROUP) == NVIC_PriorityGroup_2) || \
                                       ((GROUP) == NVIC_PriorityGroup_3) || \
                                       ((GROUP) == NVIC_PriorityGroup_4))
#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)
#define IS_NVIC_SUB_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

//VETOR_OFFSET检测宏
#define IS_VECTOR_OFFSET(OFFSET)  ((OFFSET) % 4 == 0)


//! @addtogroup SYS_Exported_Functions
//! @{

void SystemClockSetup(SYS_ClockSourceSelect_TypeDef clockSource, SYS_CoreClockSelect_TypeDef coreClock);
void SystemSoftReset(void);                                     
void GetCPUInfo(void);                                                                         
void SetVectorTable(uint32_t offset);                                                 
void NVIC_Init(IRQn_Type IRQn,uint32_t PriorityGroup,uint32_t PreemptPriority,uint32_t SubPriority);
uint16_t GetFWVersion(void);

#ifdef __cplusplus
}
#endif

#endif

//! @}

//! @}

//! @}
