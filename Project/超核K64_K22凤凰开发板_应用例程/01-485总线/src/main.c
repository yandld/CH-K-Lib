#include "gpio.h"
#include "common.h"
#include "uart.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */
 
/*
     实验名称：485总线
     实验平台；凤凰开发板
     板载芯片：MK64FN1MVLQ12
 实验效果：类似与串口，通过开发板上的485通信接口通信  
        485通信采用3.3v的MAX3485或SP3485，使用PTA25引脚控制数据状态，使用PTE24/25数据收发，
				由于板载引脚复用，需要进行片选信号设置E26/E27两个引脚的电平为0.
*/
/* 发送一串字符 */
static void UART_SendString(uint32_t instance, char * str)
{
    while(*str != '\0')
    {
        UART_WriteByte(instance, *str++);
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);//LED灯
	  GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);//485功能选择S0
	  GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_OPP);//485功能选择S1
	  GPIO_QuickInit(HW_GPIOA, 25, kGPIO_Mode_OPP);//485发送控制
	
	  GPIO_WriteBit(HW_GPIOE, 26, 0);//485功能选择S0=0
	  GPIO_WriteBit(HW_GPIOE, 27, 0);//485功能选择S1=0
	  GPIO_WriteBit(HW_GPIOA, 25, 0);//设置485处于接收状态
    /* 调试串口 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    /* 485 和 普通的TTL 串口 只是后级硬件不同 对于MCU来说，都是串口 */
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200);
    printf("485 test! please connnect other 485 device\r\n");
    
    /* 向485总线发送一串字符 */
    UART_SendString(HW_UART4, "485 device test\r\n");
    uint16_t ch;
    while(1)
    {
        /* 将485总线的数据发回去 */
        if(UART_ReadByte(HW_UART4, &ch) == 0)
        {
            GPIO_WriteBit(HW_GPIOE, 26, 1); //设置485芯片处于发送状态
					  UART_WriteByte(HW_UART4, ch);
					  GPIO_WriteBit(HW_GPIOE, 26, 0); //设置485芯片处于接收状态
        }
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
}


