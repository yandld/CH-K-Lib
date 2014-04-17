#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("ADC temperature sensor test\r\n");
    
    /* 初始化ADC模块 通道26 芯片内连接内部温度传感器 */
    ADC_InitTypeDef AD_InitStruct1;
    AD_InitStruct1.instance = HW_ADC0;
    AD_InitStruct1.clockDiv = kADC_ClockDiv8;
    AD_InitStruct1.resolutionMode = kADC_SingleDiff12or13;
    AD_InitStruct1.triggerMode = kADC_TriggerSoftware; /* 软件触发转换 */
    AD_InitStruct1.singleOrDiffMode = kADC_Differential; /*单端模式 */
    AD_InitStruct1.continueMode = kADC_ContinueConversionDisable; 
    AD_InitStruct1.hardwareAveMode = kADC_HardwareAverage_32;
    ADC_Init(&AD_InitStruct1);
    
    /* 初始化对应引脚 */
    /* 26通道 芯片内部连接到了 片内温度计上 无需引脚 */
    
    /* 启动一次ADC转换 */
    ADC_StartConversion(HW_ADC0, 26, kADC_MuxA);
    float vtemp;
    float temp;
    while(1)
    {
        ADC_StartConversion(HW_ADC0, 26, kADC_MuxA);
        while(ADC_IsConversionCompleted(HW_ADC0, kADC_MuxA));
        vtemp = ((float)ADC_ReadValue(HW_ADC0, kADC_MuxA)/4096)*3.3;
        if(vtemp >= 0.7012)
        {
            temp = (vtemp-0.7012)/.001646;
        }
        else
        {
            temp = (vtemp-0.7012)/.001769;
        }
        temp=25 - temp;
        printf("tempature:%2.3fC  \r", temp);   
        DelayMs(100);
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


