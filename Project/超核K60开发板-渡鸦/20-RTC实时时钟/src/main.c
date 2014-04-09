#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "rtc.h"

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    /* 声明一个时间结构 */
    RTC_DateTime_Type td;
    td.day = 20;
    td.hour = 23;
    td.minute = 59;
    td.second = 50;
    td.year = 2014;
    td.month = 11;
    printf("RTC test\r\n");
    
    /* 快速初始化 RTC模块 如果原来没有时间设定 则写入默认的时间设定 有则忽略这个参数 */
    RTC_QuickInit(&td);
    while(1)
    {
        /* 获得时间 */
        RTC_GetDateTime(&td);
        printf("%d-%d-%d %d:%d:%d\r\n", td.year, td.month, td.day, td.hour, td.minute, td.second);
        DelayMs(1000);
    }
}


