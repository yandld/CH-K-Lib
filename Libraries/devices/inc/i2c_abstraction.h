/**
  ******************************************************************************
  * @file    i2c_abstraction.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __I2C_ABSTRACTION_H__
#define __I2C_ABSTRACTION_H__

#include <stdint.h>


typedef enum
{
    kI2C_ABS_SpeedLow,          //!< I2C Bus: Low Speed      ( 10kHz)
    kI2C_ABS_SpeedStandard,     //!< I2C Bus: Standard Speed (100kHz)
    kI2C_ABS_SpeedFast,         //!< I2C Bus: Fast Speed     (400kHz)
    kI2C_ABS_SpeedHigh,         //!< I2C Bus: Fast+ Speed    (  1MHz)
}I2C_ABS_Speed_Type;

typedef enum
{
    kI2C_ABS_StatusOK,
    kI2C_ABS_StatusError,
    kI2C_ABS_StatusUnsupported,
    kI2C_ABS_StatusBusy,
    kI2C_ABS_StatusNoSlave,
    kI2C_ABS_StatusAribtrationLost,
}I2C_ABS_Status;

#define AT24C02_CHIP_ADDRESS  (0x50)

//!< API funtctions
int I2C_ABS_Init(int speed);
int I2C_ABS_DeInit(void);
int I2C_ABS_Probe(uint8_t chipAddress);
int I2C_ABS_AbortTransfer(void);
int I2C_ABS_ReadByte(uint8_t chipAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t *buffer, uint32_t len);
int I2C_ABS_WriteByte(uint8_t chipAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t *buffer, uint32_t len);


#endif


