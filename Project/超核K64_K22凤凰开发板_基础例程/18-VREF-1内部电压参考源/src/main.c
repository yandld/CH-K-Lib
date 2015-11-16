#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "vref.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：VREF内部电压参考源设置
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：选择芯片内部自带的电压参考源作为AD模块的参考源，进行模数转换
*/
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("VREF Test please connect ADC1_SE16 & VERF_OUT\r\n");
    /* 初始化 VREF 输出1.2V基准电压 */
    VREF_QuickInit();
    ADC_QuickInit(ADC1_SE16, kADC_SingleDiff12or13);
    uint32_t val;
    while(1)
    {
        val = ADC_QuickReadValue(ADC1_SE16);
        printf("ADC:%d | %0.3fV  \r", val, (double)val*3.300/(1<<12));
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


