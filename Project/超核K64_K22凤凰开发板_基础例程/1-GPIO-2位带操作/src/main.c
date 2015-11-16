#include "gpio.h"
#include "common.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：GPIO位带操作
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：通过使用位带操作控制开发板上的彩色小灯 
*/
//首先定义位操作的宏定义
#define LED_R  PAout(9)  //定义PTA端口的9引脚输出控制
#define LED_G  PAout(10) //定义PTA端口的10引脚输出控制
#define LED_B  PAout(11) //定义PTA端口的11引脚输出控制


int main(void)
{
    /* 初始化 Delay */
    DelayInit();

    /* 设置3个小灯的控制引脚为开漏输出 */
    GPIO_QuickInit(HW_GPIOA,  9, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 10, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOA, 11, kGPIO_Mode_OPP); 
    while(1)
    {
        /* 流水灯 */
        DelayMs(100);
        LED_R = !LED_R; //改变输出状态，原先为高电平现在输出低电平，或者相反。
        DelayMs(100);
        LED_G = !LED_G;
        DelayMs(100);
        LED_B = !LED_B;
    }
}


