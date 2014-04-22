#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ili9320.h"

#include "gui.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：LCD字符显示
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用ucgui界面，灰色界面，显示数字
*/
int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    GUI_Init(); //液晶界面初始化
    GUI_DispString("ucGUI"); //显示字符串
    GUI_DispString(GUI_GetVersionString());//获得程序版本
    GUI_DispString("\r\nHello world!");
    while(1) 
    {
        GUI_DispDecAt( i++, 20,20,4);
        if (i>9999)
        {
            i=0;
        }
    }
}


