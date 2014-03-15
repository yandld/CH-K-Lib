/**
  ******************************************************************************
  * @file    i2c_abstraction.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
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
        default:
        break;
    }
    return kI2C_ABS_StatusOK;
}


int I2C_ABS_DeInit(void)
{
    return kI2C_ABS_StatusUnsupported;
}

int I2C_ABS_AbortTransfer(void)
{
    return kI2C_ABS_StatusUnsupported;
}

int I2C_ABS_Probe(uint8_t chipAddress)
{
    int ret;
    I2C_GenerateSTART(instance);
    I2C_Send7bitAddress(instance, chipAddress, kI2C_Write);
    if(I2C_WaitAck(instance))
    {
      ret = kI2C_ABS_StatusNoSlave;
    }
    else
    {
        ret = kI2C_ABS_StatusOK;
    }
    I2C_GenerateSTOP(instance);
    while(!I2C_IsBusy(instance));
    return ret;
}

int I2C_ABS_ReadByte(uint8_t chipAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t *buffer, uint32_t len)
{
    int ret = I2C_BurstRead(instance, chipAddress, subAddress, subAddressLen, buffer, len);
    if(ret)
    {
        return kI2C_ABS_StatusError; 
    }
    return kI2C_ABS_StatusOK;
}

int I2C_ABS_WriteByte(uint8_t chipAddress, uint32_t subAddress, uint32_t subAddressLen, uint8_t *buffer, uint32_t len)
{
    int ret = I2C_BurstWrite(instance , chipAddress, subAddress, subAddressLen, buffer, len);
    if(ret)
    {
        return kI2C_ABS_StatusError; 
    }
    return kI2C_ABS_StatusOK;
}







