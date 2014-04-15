/***********************************************************************************************
//CH_Kinetis驱动库  V2.3   
//作者    :YANDLD 
//E-MAIL  :yandld@126.com
//修改日期:2013/2/14
//版本：V2.3
//淘宝：http://upcmcu.taobao.com
//QQ    1453363089
//Copyright(C) YANDLD 2012-2022
//All rights reserved
************************************************************************************************/
#ifndef __CH_LIB_CAN_H__
#define __CH_LIB_CAN_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "common.h"  

//!< hardware instances
#define HW_CAN0  (0x00U)
#define HW_CAN1  (0x01U)   
     
     
//CAN总线速度选择
typedef enum 
{
	kCAN_Baudrate_25K,
	kCAN_Baudrate_50K,
	kCAN_Baudrate_100K,
	kCAN_Baudrate_125K,
	kCAN_Baudrate_250K,
	kCAN_Baudrate_500K,
	kCAN_Baudrate_1000K,
}CAN_Baudrate_Type;

typedef enum
{
    kCAN_Frame_Remote,
    kCAN_Frame_Data,
}CAN_Frame_Type;


typedef struct
{
    uint32_t                instance;
    CAN_Baudrate_Type       baudrate;
}CAN_InitTypeDef;


//!< CAN QuickInit macro                            
#define CAN1_TX_PE24_RX_PE25   (0xB0A1U)
#define CAN0_TX_PA12_RX_PA13   (0x9880U)
#define CAN0_TX_PB18_RX_PB19   (0xA488U)
#define CAN1_TX_PC17_RX_PC16   (0xA091U)


/*!< 中断及DMA配置 */
typedef enum
{
    kCAN_IT_Tx_Disable,
    kCAN_IT_Rx_Disable,
    kCAN_IT_Tx,
    kCAN_IT_RX,
}CAN_ITDMAConfig_Type;

/*!< CAN 回调函数声明 */
typedef void (*CAN_CallBackType)(void);

//!< API functions
void CAN_SetReceiveMask(uint32_t instance, uint32_t mb, uint32_t mask);
uint32_t CAN_IsMessageBoxBusy(uint32_t instance, uint32_t mb);
uint32_t CAN_WriteData(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len);
uint32_t CAN_ReadData(uint32_t instance, uint32_t mb, uint8_t *buf, uint8_t *len);
uint32_t CAN_QuickInit(uint32_t CANxMAP, CAN_Baudrate_Type baudrate);
void CAN_ITDMAConfig(uint32_t instance, uint32_t mb, CAN_ITDMAConfig_Type config);
void CAN_SetReceiveMB(uint32_t instance, uint32_t mb, uint32_t id);
void CAN_CallbackInstall(uint32_t instance, CAN_CallBackType AppCBFun);
void CAN_Init(CAN_InitTypeDef* CAN_InitStruct);


#endif
