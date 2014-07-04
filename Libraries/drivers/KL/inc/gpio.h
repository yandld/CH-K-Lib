/**
  ******************************************************************************
  * @file    gpio.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ??????GPIO?????????
  ******************************************************************************
  */
#ifndef __CH_LIB_GPIO_H__
#define __CH_LIB_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "common.h"
#include <stdint.h>

/* GPIO???? */
#define HW_GPIOA  (0x00U)   //PTA??????
#define HW_GPIOB  (0x01U)   //PTB??????
#define HW_GPIOC  (0x02U)   //PTC??????
#define HW_GPIOD  (0x03U)   //PTD??????
#define HW_GPIOE  (0x04U)   //PTE??????
#define HW_GPIOF  (0x05U)   //PTF??????


/* ?????? ???? Signal Multiplexing and Signal Descriptions ??????*/
typedef enum
{
    kPinAlt0,  //0????
    kPinAlt1,  //1????
    kPinAlt2,  //2????
    kPinAlt3,  //3????
    kPinAlt4,  //4????
    kPinAlt5,  //5????
    kPinAlt6,  //6????
    kPinAlt7,  //7????
}PORT_PinMux_Type;

/* ??????? ,??????20K*/
typedef enum
{
    kPullDisabled,  //?????????
    kPullUp,        //????????
    kPullDown,      //????????
}PORT_Pull_Type;

/* GPIO?????? */
typedef enum
{
    kGPIO_Mode_IFT = 0x00,       //????
    kGPIO_Mode_IPD = 0x01,       //????
    kGPIO_Mode_IPU = 0x02,       //????
    kGPIO_Mode_OOD = 0x03,       //????
    kGPIO_Mode_OPP = 0x04,       //????
}GPIO_Mode_Type;

/* ?????????? */
typedef enum
{
    kInput,                  //??????
    kOutput,                 //??????
}GPIO_PinConfig_Type;

/* ?????DMA???? */
typedef enum
{
    kGPIO_DMA_RisingEdge,	      //?????DMA
    kGPIO_DMA_FallingEdge,        //?????DMA
    kGPIO_DMA_RisingFallingEdge,  //?????????DMA
    kGPIO_IT_Low,                 //???????
    kGPIO_IT_RisingEdge,          //???????
    kGPIO_IT_FallingEdge,         //???????
    kGPIO_IT_RisingFallingEdge,   //???????????
    kGPIO_IT_High,                //???????
}GPIO_ITDMAConfig_Type;

/* ???????? */
typedef struct
{
    uint8_t                instance;    //????HW_GPIOA~HW_GPIOF
    GPIO_Mode_Type         mode;        //????
    uint32_t               pinx;        //???0~31
}GPIO_InitTypeDef;

/* ?????????? */
typedef void (*GPIO_CallBackType)(uint32_t pinxArray);

/* ????? */
#define IS_GPIO_ALL_INSTANCE(INSTANCE)  (INSTANCE < ARRAY_SIZE(GPIO_InstanceTable))
#define IS_PORT_ALL_INSTANCE(INSTANCE)  (INSTANCE < ARRAY_SIZE(PORT_InstanceTable))
#define IS_GPIO_ALL_PIN(PIN)  (PIN < 32)

/* ???? ?????? ?? Cortex-M4 Generic User Guide 2.25 */
/* CM4??2?bitband?? 0x2000_0000-0x200F_FFFF ??? 0x2200_0000-0x23FF_FFFF
                         0x4000_0000-0x4000_FFFF ??? 0x4200_0000-0x43FF_FFFF
*/
#define PAout(n)   BITBAND_REG(PTA->PDOR, n)
#define PAin(n)    BITBAND_REG(PTA->PDIR, n)
  
#define PBout(n)   BITBAND_REG(PTB->PDOR, n)
#define PBin(n)    BITBAND_REG(PTB->PDIR, n)

#define PCout(n)   BITBAND_REG(PTC->PDOR, n)
#define PCin(n)    BITBAND_REG(PTC->PDIR, n)

#define PDout(n)   BITBAND_REG(PTD->PDOR, n)
#define PDin(n)    BITBAND_REG(PTD->PDIR, n)

#define PEout(n)   BITBAND_REG(PTE->PDOR, n)
#define PEin(n)    BITBAND_REG(PTE->PDIR, n)

#define PFout(n)   BITBAND_REG(PTF->PDOR, n)
#define PFin(n)    BITBAND_REG(PTF->PDIR, n)

#define PGout(n)   BITBAND_REG(PTG->PDOR, n)
#define PGin(n)    BITBAND_REG(PTG->PDIR, n)

//!< API functions
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct);
uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode);
void GPIO_WriteBit(uint32_t instance, uint8_t pinIndex, uint8_t data);
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pinIndex);
void GPIO_ToggleBit(uint32_t instance, uint8_t pinIndex);
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, bool status);
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun);
//!< low level functions
void PORT_PinPullConfig(uint32_t instance, uint8_t pinIndex, PORT_Pull_Type pull);
void GPIO_PinConfig(uint32_t instance, uint8_t pinIndex, GPIO_PinConfig_Type mode);
void PORT_PinMuxConfig(uint32_t instance, uint8_t pinIndex, PORT_PinMux_Type pinMux);
uint32_t GPIO_ReadPort(uint32_t instance);
void GPIO_WritePort(uint32_t instance, uint32_t data);
void PORT_PinOpenDrainConfig(uint32_t instance, uint8_t pinIndex, FunctionalState newState);
void PORT_PinPassiveFilterConfig(uint32_t instance, uint8_t pinIndex, FunctionalState newState);
    
#ifdef __cplusplus
}
#endif

#endif


