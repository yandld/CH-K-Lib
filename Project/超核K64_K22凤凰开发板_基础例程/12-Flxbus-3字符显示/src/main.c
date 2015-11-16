#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ili9320.h"

#include "gui.h"
/*
     实验名称：液晶屏字符显示
     实验平台：凤凰开发板
     板载芯片：MK64FN1MVLQ12
 实验效果：调用GUI接口，在液晶屏上显示文字
*/
int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flexbus lcd test\r\n");
    GUI_Init();
    GUI_DispString("emWin");
    GUI_DispString(GUI_GetVersionString());
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


