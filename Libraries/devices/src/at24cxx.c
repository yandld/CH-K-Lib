/**
  ******************************************************************************
  * @file    at24cxx.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "i2c_abstraction.h"
#include "at24cxx.h"
#include <string.h>
/**
  ******************************************************************************
  * @supported chip    AT24C01
  *                    AT24C02
  *                    AT24C04
  *                    AT24C08
  *                    AT24C16
  ******************************************************************************
  */
static const uint32_t AT24CXX_TotalSizeTable[] = {128, 256, 512, 1024, 2048};
static const uint32_t AT24CXX_PageSizeTable[] =  {8, 8, 16, 16, 16};
static int AT24CXX_Type;

#define AT24CXX_CHIP_ADDRESS   (0x50)

int AT24CXX_Init(int type)
{
    AT24CXX_Type = (int)type;
    return I2C_ABS_Init(kI2C_ABS_SpeedFast);
}

int AT24CXX_GetTotalSize(void)
{
    return AT24CXX_TotalSizeTable[AT24CXX_Type];
}

int AT24CXX_ReadByte(uint32_t address, uint8_t *buffer, uint32_t len)
{   
    if(len > AT24CXX_TotalSizeTable[AT24CXX_Type])
    {
        return kI2C_ABS_StatusError;
    }
    return I2C_ABS_ReadByte(AT24CXX_CHIP_ADDRESS+((address/256)), address%256, 1, buffer, len); 
}

static int AT24CXX_WritePage(uint32_t address, uint8_t *buffer, uint32_t len)
{
    if(len > AT24CXX_TotalSizeTable[AT24CXX_Type])
    {
        return kI2C_ABS_StatusError;
    }
    return I2C_ABS_WriteByte(AT24CXX_CHIP_ADDRESS+((address/256)), address%256, 1, buffer, len); 
}


int AT24CXX_WriteByte(uint32_t address, uint8_t *buffer, uint32_t len)
{
    int ret;
	uint32_t secpos;
	uint32_t secoff;
	uint16_t Byteremian;
	uint32_t pageremain;
	secpos = address/AT24CXX_PageSizeTable[AT24CXX_Type];
	secoff = address%AT24CXX_PageSizeTable[AT24CXX_Type];
	pageremain = AT24CXX_PageSizeTable[AT24CXX_Type]-secoff;
	ret = AT24CXX_WritePage(address, buffer, pageremain);
    if(ret)
    {
        return ret;
    }
    while(I2C_ABS_Probe(AT24CXX_CHIP_ADDRESS) != kI2C_ABS_StatusOK);
	buffer += pageremain;
	secpos++;
	Byteremian = len - pageremain;
	while(Byteremian != 0)
	{
		if(Byteremian <= AT24CXX_PageSizeTable[AT24CXX_Type])
		{
			 pageremain = Byteremian;
		}
		else
		{
			pageremain = AT24CXX_PageSizeTable[AT24CXX_Type];
		}
        
        ret = AT24CXX_WritePage(secpos*AT24CXX_PageSizeTable[AT24CXX_Type], buffer, pageremain);
        if(ret)
        {
            return ret;
        }
        while(I2C_ABS_Probe(AT24CXX_CHIP_ADDRESS) != kI2C_ABS_StatusOK);
        secpos++;
        buffer += pageremain;
        Byteremian -= pageremain;
	}
    return kI2C_ABS_StatusOK;
}

int AT24CXX_SelfTest(void)
{
    int ret;
    uint8_t buf[8],buf1[8],buf2[8];
    ret = AT24CXX_ReadByte(0, buf, sizeof(buf));
    if(ret)
    {
        return ret;
    }
    memset(buf1,'T',sizeof(buf1));
    ret = AT24CXX_WriteByte(0, buf1, sizeof(buf1));
    if(ret)
    {
        return ret;
    }
    ret = AT24CXX_ReadByte(0, buf2, sizeof(buf2));
    if(ret)
    {
        return ret;
    }
    ret = AT24CXX_WriteByte(0, buf, sizeof(buf));
    if(ret)
    {
        return ret;
    }
    if(memcmp(buf1, buf2, sizeof(buf1)))
    {
        return kI2C_ABS_StatusError;
    }
    return kI2C_ABS_StatusOK;
}


