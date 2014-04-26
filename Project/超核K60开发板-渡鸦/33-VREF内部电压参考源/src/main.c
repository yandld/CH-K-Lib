#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "vref.h"

int main(void)
{
    uint32_t i;
    uint16_t value[16];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("VREF Test please connect ADC1_SE16 & VERF_OUT\r\n");
    /* 初始化 VREF 输出1.2V基准电压 */
    VREF_QuickInit();
    
    ADC_QuickInit(ADC1_SE16, kADC_SingleDiff12or13);

    
    while(1)
    {
        printf("ADC:%d\r\n", ADC_QuickReadValue(ADC1_SE16));
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


