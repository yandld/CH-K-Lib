/**
  ******************************************************************************
  * @file    flexbus.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片FlexBus模块的底层功能函数
  ******************************************************************************
  */
#ifndef __FLEXBUS_H__
#define __FLEXBUS_H__

#include <stdint.h>
#include "common.h"

//!< Flexbus 数据对其方式选择
#define kFLEXBUS_DataLeftAligned   (0x00)
#define kFLEXBUS_DataRightAligned  (0x01)

//!< Flexbus 自动应答信号使能
#define kFLEXBUS_AutoAckEnable     (0x00)
#define kFLEXBUS_AutoAckDisable    (0x01)

//!< Flexbus 端口位宽选择
#define kFLEXBUS_PortSize_8Bit     (0x01)
#define kFLEXBUS_PortSize_16Bit    (0x02)
#define kFLEXBUS_PortSize_32Bit    (0x00)

//!< Flexbus 片选信号选择 
#define kFLEXBUS_CS0               (0x00)
#define kFLEXBUS_CS1               (0x01)
#define kFLEXBUS_CS2               (0x02)
#define kFLEXBUS_CS3               (0x03)
#define kFLEXBUS_CS4               (0x04)
#define kFLEXBUS_CS5               (0x05)

//!< Flexbus 片选范围 参考选择
#define kFLEXBUS_ADSpace_64KByte        (0x00)
#define kFLEXBUS_ADSpace_128KByte       (0x01)
#define kFLEXBUS_ADSpace_512KByte       (0x07)
#define kFLEXBUS_ADSpace_1MByte         (0x0F)

//!< Flexbus BE信号控制模式选择
#define kFLEXBUS_BE_AssertedWrite       (0x00)
#define kFLEXBUS_BE_AssertedReadWrite   (0x01)

//!< Flexus 控制信号复用选择
#define kFLEXBUS_CSPMCR_GROUP1_ALE      (0x00)
#define kFLEXBUS_CSPMCR_GROUP1_CS1      (0x01)
#define kFLEXBUS_CSPMCR_GROUP1_TS       (0x02)
#define kFLEXBUS_CSPMCR_GROUP2_CS4      (0x00)
#define kFLEXBUS_CSPMCR_GROUP2_TSIZ0    (0x01)
#define kFLEXBUS_CSPMCR_GROUP2_BE_31_24 (0x02)
#define kFLEXBUS_CSPMCR_GROUP3_CS5      (0x00)
#define kFLEXBUS_CSPMCR_GROUP3_TSIZ1    (0x01)
#define kFLEXBUS_CSPMCR_GROUP3_BE_23_16 (0x02)
#define kFLEXBUS_CSPMCR_GROUP4_TBST     (0x00)
#define kFLEXBUS_CSPMCR_GROUP4_CS2      (0x01)
#define kFLEXBUS_CSPMCR_GROUP4_BE_15_8  (0x02)
#define kFLEXBUS_CSPMCR_GROUP5_TA       (0x00)
#define kFLEXBUS_CSPMCR_GROUP5_CS3      (0x01)
#define kFLEXBUS_CSPMCR_GROUP5_BE_7_0   (0x02)

//!< FLEXBUS初始化结构体
typedef struct
{
    uint32_t dataWidth;      //总线宽度 8/16/32
    uint32_t baseAddress;    //32位基地址
    uint32_t ADSpaceMask;    //片选数据空间设置
    uint32_t dataAlignMode;  //数据对其方式
    uint32_t autoAckMode;    //自动应答模式
    uint32_t ByteEnableMode; //位使能模式选择
    uint32_t CSn;            //片选信号设置
    uint32_t CSPortMultiplexingCotrol; //多功能引脚配置
}FLEXBUS_InitTypeDef;

//!< API functions
void FLEXBUS_Init(FLEXBUS_InitTypeDef* FLEXBUS_InitStruct);


#endif

