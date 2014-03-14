/**
  ******************************************************************************
  * @file    i2c_abstraction.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "i2c_abstraction.h"
#include "i2c.h"
#include "board.h"


static uint8_t instance;

int I2C_ABS_Init(int speed)
{
    switch((I2C_ABS_Speed_Type)speed)
    {
        case kI2C_ABS_SpeedLow:
            instance = I2C_QuickInit(BOARD_I2C_MAP, 10*1000);
            break;
        case kI2C_ABS_SpeedStandard:
            instance = I2C_QuickInit(BOARD_I2C_MAP, 100*1000);
            break;
        case kI2C_ABS_SpeedFast:
            instance = I2C_QuickInit(BOARD_I2C_MAP, 400*1000);
            break;
        case kI2C_ABS_SpeedHigh:
            instance = I2C_QuickInit(BOARD_I2C_MAP, 1000*1000);
            break;
        default:break;
    }
}


int I2C_ABS_DeInit(void)
{
    
}

int I2C_ABS_AbortTransfer(void)
{
    
}

int I2C_ABS_Probe(uint8_t chipAddress)
{
    
    
}

int I2C_ABS_ReadByte(uint8_t chipAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t *buffer, uint32_t len)
{
    I2C_BurstRead(instance, chipAddress, subAddress, subAddressLen, buffer, len);
}

int I2C_ABS_WriteByte(uint8_t chipAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t *buffer, uint32_t len)
{
    I2C_BurstWrite(instance , chipAddress, subAddress, subAddressLen, buffer, len);
}







