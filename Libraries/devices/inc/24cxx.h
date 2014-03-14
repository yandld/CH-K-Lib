/**
  ******************************************************************************
  * @file    24cxx.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __24CXX_H__
#define __24CXX_H__

#include <stdint.h>



typedef enum
{
    kAT24C01,
    kAT24C02,
    kAT24C04,
    kAT24C08,
    kAT24C16,
}AT24CXX_Device_Type;

//API funtctions
void AT24CXX_Init(int type);
void AT24CXX_ReadByte(uint32_t address, uint8_t *buffer, uint32_t len);
void AT24CXX_WriteByte(uint32_t address, uint8_t *buffer, uint32_t len);
uint32_t AT24CXX_GetTotalSize(AT24CXX_Device_Type type);


#endif

