#include "gpio.h"
#include "common.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* GPIO 工作模式 */
/*
 *         @arg kGPIO_Mode_IFT :悬空输入
 *         @arg kGPIO_Mode_IPD :下拉输入
 *         @arg kGPIO_Mode_IPU :上拉输入
 *         @arg kGPIO_Mode_OOD :开漏输出 如果不清楚这2种输出的区别请 百度..
 *         @arg kGPIO_Mode_OPP :推挽输出
 */
 
/*
     实验名称：GPIO小灯实验
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：控制PTA端口的9引脚是输出模式
      在低电平时小灯亮起，发出绿光，在高电平时小灯熄灭
      小灯周期性闪烁，闪烁时间间隔500ms     
*/
 
int main(void)
{
    /* 初始化Delay */
    DelayInit();
    /* 使用简易初始化初始化PTA端口的9引脚作为IO使用 设置为推挽输出模式 */
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    /* 另外一种初始化方式: 结构体模式的初始化 有点类似STM32固件库*/
//    GPIO_InitTypeDef GPIO_InitStruct1;
//    GPIO_InitStruct1.instance = HW_GPIOA;
//    GPIO_InitStruct1.mode = kGPIO_Mode_OPP;
//    GPIO_InitStruct1.pinx = 9;
//    GPIO_Init(&GPIO_InitStruct1);
    
    /* 控制PTA端口的9引脚输出低电平 */
    GPIO_WriteBit(HW_GPIOA, 9, 0);
    
    while(1)
    {
//        /* 翻转引脚电平 原来是低变成高 原来是高变成低 */
//        GPIO_ToggleBit(HW_GPIOA, 9);
        /* 另外一种方法 使用位带操作 */
        PAout(9) = !PAout(9);
        DelayMs(500);
    }
}


