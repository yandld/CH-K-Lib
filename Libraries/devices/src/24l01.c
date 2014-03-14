/**
  ******************************************************************************
  * @file    24l01.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "24l01.h"
#include "spi.h"




uint8_t NRF2401_Init(void)
{
    /*
	SPI_InitTypeDef SPI_InitStruct1;
	//CE引脚初始化
	GPIO_QuickInit(HW_GPIOB, 0 , kGPIO_Mode_OPP);   //Init CE
    CE=0;
	 //IRQ引脚初始化
	GPIO_QuickInit(HW_GPIOB, 1 , kGPIO_Mode_IPU);   //Init IRQ
	//初始化SPI串行接口引擎
	SPI_InitStruct1.SPIxDataMap = SPI0_SCK_PC5_SOUT_PC6_SIN_PC7;
	SPI_InitStruct1.SPIxPCSMap =  SPI0_PCS2_PC2;
	SPI_InitStruct1.SPI_DataSize = 8;
	SPI_InitStruct1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStruct1.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct1.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct1.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct1.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(&SPI_InitStruct1);
	//初始化硬件GPIO引擎
	//不适用硬件 CS 使用GPIO做CS
	GPIO_QuickInit(HW_GPIOC, 2 , kGPIO_Mode_OPP);   //Init CS
	//IO电平初始化
	CE =0;// chip enable
	CS =1;// cs disable
	//检测NRF2401 是否存在
	return 0;
    */
}

