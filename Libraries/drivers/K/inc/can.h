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




typedef struct
{
    uint32_t                instance;
    CAN_Baudrate_Type       baudrate;
}CAN_InitTypeDef;     
     

//kFlexCanBitrate_125k
//!< CAN QuickInit macro                            
#define CAN1_TX_PE24_RX_PE25   (0xB0A1U)
#define CAN0_TX_PA12_RX_PA13   (0x9880U)
#define CAN0_TX_PB18_RX_PB19   (0xA488U)
#define CAN1_TX_PC17_RX_PC16   (0xA091U)

//枚举FlexCAN模块邮箱的CODE段的数值
typedef enum CAN_Msg_Code_Tag
{
  CAN_MSGOBJ_RX_INACTIVE      = 0x0,  //@接收不激活
  CAN_MSGOBJ_RX_BUSY          = 0x1,  //@接收忙
  CAN_MSGOBJ_RX_FULL          = 0x2,  //@接收满
  CAN_MSGOBJ_RX_EMPTY         = 0x4,  //@emem RX Empty
  CAN_MSGOBJ_RX_OVERRUN       = 0x6,  //@emem RX Overrun
  CAN_MSGOBJ_TX_INACTIVE      = 0x8,  //@emem TX Inactive
  CAN_MSGOBJ_TX_REMOTE        = 0xA,  //@emem TX Remote
  CAN_MSGOBJ_TX_ONCE          = 0xC,  //@emem TX Unconditional
  CAN_MSGOBJ_TX_REMOTE_MATCH  = 0xE   //@emem Tx Remote match
} CAN_Msg_CodeEnum_Type ;


//默认设置
#define CAN_DEFAULT_TXID               (8)
#define CAN_DEFAULT_RXID               (5)
#define CAN_DEFAULT_RXID1              (6)
#define CAN_DEFAULT_RXMSGBOXINDEX      (5)



#define CAN_get_code(cs)			(((cs) & CAN_CS_CODE_MASK)>>24)
#define CAN_get_length(cs)    (((cs) & CAN_CS_DLC_MASK)>>16)
//本构件实现的接口函数


void CAN_EnableRev(CAN_Type *can, uint8_t boxindex, uint32_t rxid);
uint8_t CAN_SendData(CAN_Type *can,uint8_t boxindex, uint32_t txid, uint8_t *Data, uint8_t len);
uint8_t CAN_ReadData(CAN_Type *can,uint8_t boxindex, uint8_t *Data);
void CAN_EnableInterrupts(CAN_Type * can);
void CAN_DisableInterrupts(CAN_Type * can);


void CAN_QuickInit(uint32_t CANxMAP, CAN_Baudrate_Type baudrate);


#endif
