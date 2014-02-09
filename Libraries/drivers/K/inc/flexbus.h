/**
  ******************************************************************************
  * @file    flexbus.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "common.h"

#define kFLEXBUS_DataLeftAligned   (0x00)
#define kFLEXBUS_DataRightAligned  (0x01)

#define kFLEXBUS_AutoAckEnable     (0x00)
#define kFLEXBUS_AutoAckDisable    (0x01)

#define kFLEXBUS_PortSize_8Bit     (0x01)
#define kFLEXBUS_PortSize_16Bit    (0x10)
#define kFLEXBUS_PortSize_32Bit    (0x00)

#define kFLEXBUS_CS0               (0x00)
#define kFLEXBUS_CS1               (0x01)
#define kFLEXBUS_CS2               (0x02)
#define kFLEXBUS_CS3               (0x03)
#define kFLEXBUS_CS4               (0x04)
#define kFLEXBUS_CS5               (0x05)

typedef struct
{
    uint32_t portSize;
    uint32_t chipSelectAddress;
    uint32_t addressSpaceInByte;
    uint32_t dataAlignMode;
    uint32_t autoAckMode;
    uint32_t chipSelectChl;
    
}FLEXBUS_InitTypeDef;