#include "gpio.h"
#include "common.h"
#include "uart.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：GPIO外部引脚中断
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
		 实验效果: 当按键KEY3（PTA7）每按一次另一颗小灯将闪烁一次
          通过按键控制红色小灯的亮灭。对按键的引脚采用下降沿触发中断的方式。
*/

/* 回调函数中的 array 的32位每一位代表一个引脚，1为 该脚中断被触发 0代表该脚中断未触发 */
static void GPIO_ISR(uint32_t array)
{
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, false);
	
    if(array & (1 << 7)) /*对应的按键中断 翻转对应的LED电平 */
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP); /* LED */
	
    /*配置PTA端口的7引脚为上拉输入模式 */
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU); /* KEY */
   
    /* 设置GPIO外部引脚中断回调函数 */
    GPIO_CallbackInstall(HW_GPIOA, GPIO_ISR);
    /* 打开PTA7引脚的中断 上升沿触发 */
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
    
    while(1)
    {
        DelayMs(500);
    }
}


