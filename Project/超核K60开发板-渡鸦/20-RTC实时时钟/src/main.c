#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "rtc.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称: RTC实时时钟
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：通过串口发送时钟时间   
*/
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    /* 声明一个时间结构 */
    RTC_DateTime_Type td;
    td.day = 20;    //设置日 
    td.hour = 23;   //设置时
    td.minute = 59; //设置分 
    td.second = 50; //设置秒 
    td.year = 2014; //设置年 
    td.month = 11;  //设置月 
    printf("RTC test\r\n");
    
    /* 快速初始化 RTC模块 如果原来没有时间设定 则写入默认的时间设定 有则忽略这个参数 */
    RTC_QuickInit(&td);
    while(1)
    {
        /* 获得时间 */
        RTC_GetDateTime(&td);//获得时间
        printf("%d-%d-%d %d:%d:%d\r\n", td.year, td.month, td.day, td.hour, td.minute, td.second);
        DelayMs(1000);
    }
}


