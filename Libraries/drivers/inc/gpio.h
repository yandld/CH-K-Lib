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

//! @brief GPIO_Bit action :low or high state
typedef enum
{
    Bit_RESET = 0,           //!< Reset, low  state
    Bit_SET                  //!< Set  , high state
}BitAction;
#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || ((ACTION) == Bit_SET))

//! @brief GPIO_Pin select
typedef enum
{
    kGPIO_Pin_0,
    kGPIO_Pin_1,
    kGPIO_Pin_2,
    kGPIO_Pin_3,
    kGPIO_Pin_4,
    kGPIO_Pin_5,
    kGPIO_Pin_6,
    kGPIO_Pin_7,
    kGPIO_Pin_8,
    kGPIO_Pin_9,
    kGPIO_Pin_10,
    kGPIO_Pin_11,
    kGPIO_Pin_12,
    kGPIO_Pin_13,
    kGPIO_Pin_14,
    kGPIO_Pin_15,
    kGPIO_Pin_16,
    kGPIO_Pin_17,
    kGPIO_Pin_18,
    kGPIO_Pin_19,
    kGPIO_Pin_20,
    kGPIO_Pin_21,
    kGPIO_Pin_22,
    kGPIO_Pin_23,
    kGPIO_Pin_24,
    kGPIO_Pin_25,
    kGPIO_Pin_26,
    kGPIO_Pin_27,
    kGPIO_Pin_28,
    kGPIO_Pin_29,
    kGPIO_Pin_30,
    kGPIO_Pin_31,
}GPIO_PinSelect_TypeDef;

typedef enum
{
    kGPIO_Mode_IFT = 0x04,            //!< input floating mode
    kGPIO_Mode_IPD = 0x05,            //!< input pull down mode
    kGPIO_Mode_IPU = 0x06,            //!< input pull up mode
    kGPIO_Mode_OOD = 0x07,            //!< output open drain mode
    kGPIO_Mode_OPP = 0x08,            //!< output push mode
}GPIO_ModeSelect_TypeDef;


typedef enum
{
    kGPIO_IT_Low = 0x08,               //!< Trigger interrupt when GPIO external pin is in low state
    kGPIO_IT_Rising = 0x09,            //!< Trigger interrupt when GPIO external pin dectect rising edge
    kGPIO_IT_Falling = 0x0A,           //!< Trigger interrupt when GPIO external pin dectect falling edge
    kGPIO_IT_RisingAndFalling = 0x0B,  //!< Trigger interrupt when GPIO external pin dectect rising or falling edge
    kGPIO_IT_High = 0x0C,              //!< Trigger interrupt when GPIO external pin is in high state
}GPIO_ITSelect_TypeDef;

typedef enum
{
    kGPIO_DMA_Rising = 0x01,           //!< Trigger DMA when GPIO external pin detect rising edge
    kGPIO_DMA_Falling = 0x02,          //!< Trigger DMA when GPIO external pin detect falling edge
    kGPIO_DMA_RisingAndFalling = 0x03, //!< Trigger DMA when GPIO external pin detect rising or falling edge
}GPIO_DMASelect_TypeDef;

typedef struct
{
    GPIO_PinSelect_TypeDef GPIO_Pin;    //!< GPIO pin select
    BitAction GPIO_InitState;           //!< In GPIO config into output mode, this flied determinate higt or low state, when GPIO config to input mode, this field is no use.
    GPIO_ModeSelect_TypeDef GPIO_Mode;  //!< GPIO operation mode
    GPIO_Type *GPIOx;                   //!< GPIO port : PTA-PTE
}GPIO_InitTypeDef;

//! @}

//! @defgroup GPIO_Exported_Functions
//! @{

void GPIO_Init(GPIO_InitTypeDef* GPIO_InitStruct);
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
ITStatus GPIO_GetITStates(GPIO_Type *GPIOx,GPIO_PinSelect_TypeDef GPIO_Pin);
void GPIO_ClearITPendingBit(GPIO_Type *GPIOx,uint16_t GPIO_Pin);
void GPIO_ITConfig(GPIO_Type* GPIOx, GPIO_ITSelect_TypeDef GPIO_IT, GPIO_PinSelect_TypeDef GPIO_Pin, FunctionalState NewState);
void GPIO_DMACmd(GPIO_Type* GPIOx, GPIO_DMASelect_TypeDef GPIO_DMAReq, GPIO_PinSelect_TypeDef GPIO_Pin, FunctionalState NewState);


#ifdef __cplusplus
}
#endif

#endif

//! @}

//! @}

//! @}
