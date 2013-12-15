/**
  ******************************************************************************
  * @file    gpio.h
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.25
  * @brief   超核K60固件库 GPIO API函数 头文件
  ******************************************************************************
  */
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "common.h"

//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup GPIO
//! @brief GPIO driver modules
//! @{

typedef enum
{
    kGPIO_Mode_IFT = 0x00,            //!< input floating mode
    kGPIO_Mode_IPD = 0x01,            //!< input pull down mode
    kGPIO_Mode_IPU = 0x02,            //!< input pull up mode
    kGPIO_Mode_OOD = 0x03,            //!< output open drain mode
    kGPIO_Mode_OPP = 0x04,            //!< output push mode
		kGPIO_ModeNameCount,
}GPIO_Mode_Type;

typedef enum
{
    HW_GPIOA,
    HW_GPIOB,
    HW_GPIOC,
    HW_GPIOD,
    HW_GPIOE,
    HW_GPIOF,
		HW_GPIONameCount,
}GPIO_Instance_Type;

typedef enum
{
    kPinAlt0,
    kPinAlt1,
    kPinAlt2,
    kPinAlt3,
    kPinAlt4,
    kPinAlt5,
    kPinAlt6,
    kPinAlt7,
    kPinAltNameCount,
}PORT_PinMux_Type;

typedef enum
{
    kPullDisabled,
    kPullUp,
    kPullDown,
    kPullNameCount,
}PORT_Pull_Type;


typedef enum
{
    kInpput,
    kOutput,
    kPinConfigNameCount,
}GPIO_PinConfig_Type;

//位带操作,实现51类似的GPIO控制功能
//IO口操作宏定义
#define BITBAND(addr,bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (PTA_BASE+0) //0x4001080C 
#define GPIOB_ODR_Addr    (PTB_BASE+0) //0x40010C0C 
#define GPIOC_ODR_Addr    (PTC_BASE+0) //0x4001100C 
#define GPIOD_ODR_Addr    (PTD_BASE+0) //0x4001140C 
#define GPIOE_ODR_Addr    (PTE_BASE+0) //0x4001180C 
#define GPIOF_ODR_Addr    (PTF_BASE+0) //0x40011A0C    
#define GPIOG_ODR_Addr    (PTG_BASE+0) //0x40011E0C    

#define GPIOA_IDR_Addr    (PTA_BASE+0x10) //0x40010808 
#define GPIOB_IDR_Addr    (PTB_BASE+0x10) //0x40010C08 
#define GPIOC_IDR_Addr    (PTC_BASE+0x10) //0x40011008 
#define GPIOD_IDR_Addr    (PTD_BASE+0x10) //0x40011408 
#define GPIOE_IDR_Addr    (PTE_BASE+0x10) //0x40011808 
#define GPIOF_IDR_Addr    (PTF_BASE+0x10) //0x40011A08 
#define GPIOG_IDR_Addr    (PTG_BASE+0x10) //0x40011E08 



//! @addtogroup GPIO_Constants_Macros
//! @{


//IO口操作,只对单一的IO口!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //! < output
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //! < input
  
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

//! @}

#define IS_GPIO_ALL_PERIPH(PERIPH) (((PERIPH) == PTA) || \
                                    ((PERIPH) == PTB) || \
                                    ((PERIPH) == PTC) || \
                                    ((PERIPH) == PTD) || \
                                    ((PERIPH) == PTE))


#define IS_PORT_ALL_PERIPH(PERIPH) (((PERIPH) == PORTA) || \
                                    ((PERIPH) == PORTB) || \
                                    ((PERIPH) == PORTC) || \
                                    ((PERIPH) == PORTD) || \
                                    ((PERIPH) == PORTE))

//! @addtogroup GPIO_Exported_Types
//! @{




typedef enum
{
    kGPIO_ITDMA_Disable = 0x00,
    kGPIO_DMA_RisingEdge = 0x01,	
    kGPIO_DMA_FallingEdge = 0x02,
    kGPIO_DMA_RisingFallingEdge = 0x03,
    kGPIO_IT_Low = 0x08,
    kGPIO_IT_RisingEdge = 0x09,
    kGPIO_IT_FallingEdge = 0x0A,
    kGPIO_IT_RisingFallingEdge = 0x0B,
    kGPIO_IT_High = 0x0C,
		kGPIO_ITDMAConfigNameCount,
}GPIO_ITDMAConfig_Type;



typedef struct
{
    GPIO_Instance_Type     instance;             //!< GPIO pin select
		GPIO_Mode_Type         mode;                 //!< GPIO operation mode
		uint32_t               pinx;                 //!< pin index
}GPIO_InitTypeDef;


typedef void (*GPIO_CallBackType)(uint32_t pinxArray);
//! @}

//! @defgroup GPIO_Exported_Functions
//! @{

State_Type GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct);
State_Type PORT_PinMuxConfig(GPIO_Instance_Type instance, uint8_t pinIndex, PORT_PinMux_Type pinMux);
/*
void GPIO_WriteBit(GPIO_Type *GPIOx,uint16_t GPIO_Pin,BitAction BitVal);
void GPIO_SetBits(GPIO_Type* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_Type* GPIOx, uint16_t GPIO_Pin);
void GPIO_ToggleBit(GPIO_Type *GPIOx,uint16_t GPIO_Pin);
void GPIO_WriteData(GPIO_Type *GPIOx,uint32_t PortVal);
uint8_t GPIO_ReadOutputDataBit(GPIO_Type* GPIOx, uint16_t GPIO_Pin);
uint32_t GPIO_ReadOutputData(GPIO_Type* GPIOx);
uint8_t GPIO_ReadInputDataBit(GPIO_Type* GPIOx, uint16_t GPIO_Pin);
uint32_t GPIO_ReadInputData(GPIO_Type *GPIOx);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
ITStatus GPIO_GetITStates(GPIO_Type *GPIOx,GPIO_Pin_Type GPIO_Pin);
void GPIO_ClearITPendingBit(GPIO_Type *GPIOx,uint16_t GPIO_Pin);
void GPIO_ITConfig(GPIO_Type* GPIOx, GPIO_ITSelect_TypeDef GPIO_IT, GPIO_Pin_Type GPIO_Pin, FunctionalState NewState);
void GPIO_DMACmd(GPIO_Type* GPIOx, GPIO_DMASelect_TypeDef GPIO_DMAReq, GPIO_Pin_Type GPIO_Pin, FunctionalState NewState);
*/

#ifdef __cplusplus
}
#endif

#endif

//! @}

//! @}

//! @}
