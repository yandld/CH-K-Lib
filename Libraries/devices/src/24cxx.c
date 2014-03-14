/**
  ******************************************************************************
  * @file    24cxx.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "i2c_abstraction.h"
#include "24cxx.h"
#include "systick.h"


static const uint32_t AT24CXX_TotalSizeTable[] = {128, 256, 512, 1024, 2048};
static const uint32_t AT24CXX_PageSizeTable[] =  {8, 8, 16, 16, 16};
static int gDeviceType;

#define AT24CXX_CHIP_ADDRES   (0x50)

void AT24CXX_Init(int type)
{
    I2C_ABS_Init(kI2C_ABS_SpeedLow);
    gDeviceType = (int)type;
}

uint32_t AT24CXX_GetTotalSize(AT24CXX_Device_Type type)
{
    return AT24CXX_TotalSizeTable[type];
}

void AT24CXX_ReadByte(uint32_t address, uint8_t *buffer, uint32_t len)
{   
    I2C_ABS_ReadByte(AT24CXX_CHIP_ADDRES+((address/256)), address, 1, buffer, len); 
}

static int AT24CXX_WritePage(uint32_t address, uint8_t *buffer, uint32_t len)
{
    I2C_ABS_WriteByte(AT24CXX_CHIP_ADDRES+((address/256)), address, 1, buffer, len); 
}

void AT24CXX_WriteByte(uint32_t address, uint8_t *buffer, uint32_t len)
{
	uint32_t secpos;
	uint32_t secoff;
	uint16_t Byteremian;
	uint32_t pageremain;
	secpos = address/AT24CXX_PageSizeTable[gDeviceType];
	secoff = address%AT24CXX_PageSizeTable[gDeviceType];
	pageremain = AT24CXX_PageSizeTable[gDeviceType]-secoff;
	AT24CXX_WritePage(address, buffer, pageremain);
	DelayMs(1);
	buffer += pageremain;
	secpos++;
	Byteremian = len-pageremain;
	while(Byteremian != 0)
	{
		if(Byteremian <= AT24CXX_PageSizeTable[gDeviceType])
		{
			 pageremain = Byteremian;
		}
		else
		{
			pageremain = AT24CXX_PageSizeTable[gDeviceType];
		}
        
		AT24CXX_WritePage(secpos*AT24CXX_PageSizeTable[gDeviceType], buffer, pageremain);
		DelayMs(1);
		secpos++;
		buffer += pageremain;
		Byteremian -= pageremain;
	}
}

