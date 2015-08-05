/**
  ******************************************************************************
  * @file    ssd1306.c
  * @author  YANDLD
  * @date    2015.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include <string.h>
#include "ssd1306.h"
#include "i2c.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#define SSD1306_ADDR        (0x3C)
void WriteCmd(unsigned char I2C_Command)//写命令
{
	I2C_WriteSingleRegister(1, SSD1306_ADDR, 0x00, I2C_Command);
}
void WriteDat(unsigned char I2C_Data)//写数据
{
	I2C_WriteSingleRegister(1, SSD1306_ADDR, 0x40, I2C_Data);
}



void ssd1306_init(void)
{
	DelayMs(100); //这里的延时很重要
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //亮度调节 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
    int i;
    uint8_t buf[128];
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
        WriteCmd(0x10);		//high column start address
        for(i=0; i<sizeof(buf); i++)
        {
            buf[i] = fill_Data;
        }
        I2C_BurstWrite(0, SSD1306_ADDR, 0x40, 1, buf, sizeof(buf));
	}
}



