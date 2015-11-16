#include "gpio.h"
#include "common.h"
#include "uart.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：SMC低功耗STOP
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：控制开发板上的3个普通按键，当按键按下时
        引脚信号为低电平，采用轮询的方式读取按键的状态
        并使用小灯的亮灭展示按键的效果
*/

/* 回调函数中的 array 的32位每一位代表一个引脚，1为 该脚中断被触发 0代表该脚中断未触发 */
static void GPIO_ISR(uint32_t array)
{
    printf("enter interrupt, back to RUN now!\r\n");
}

int main(void)
{
    DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    GPIO_QuickInit(HW_GPIOA, 6, kGPIO_Mode_IPU);
    GPIO_CallbackInstall(HW_GPIOA, GPIO_ISR);
    GPIO_ITDMAConfig(HW_GPIOA, 6, kGPIO_IT_FallingEdge, true);
    
    printf("low power STOP mode\r\n");
    
    while(1)
    {
        EnterSTOPMode(false);
        printf("Wake Up!\r\n");
    }
}


