#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "pit.h"


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("PIT Trigger ADC test\r\n");
    
    /* 将ADC0 的 触发源设置为 PIT0 触发 */
    SIM->SOPT7 |= SIM_SOPT7_ADC0TRGSEL(4);
    
    /* 初始化ADC模块 ADC0_SE19_BM0 */
    ADC_InitTypeDef AD_InitStruct1;
    AD_InitStruct1.instance = HW_ADC0;
    AD_InitStruct1.chl = 19;
    AD_InitStruct1.clockDiv = kADC_ClockDiv2; /* ADC采样时钟2分频 */
    AD_InitStruct1.resolutionMode = kADC_SingleDiff10or11; /*单端 10位精度 查分 11位精度 */
    AD_InitStruct1.triggerMode = kADC_TriggerHardware; /* 硬件触发转换 */
    AD_InitStruct1.singleOrDiffMode = kADC_Single; /*单端模式 */
    AD_InitStruct1.continueMode = kADC_ContinueConversionDisable; 
    AD_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable; /*禁止 硬件平均 功能 */
    ADC_Init(&AD_InitStruct1);
    
    /* 初始化对应引脚 */
    /* DM0引脚为专门的模拟引脚 ADC时 无需设置复用  DM0也无法当做普通的数字引脚 */
    
    /* 初始化PIT0 */
    PIT_QuickInit(HW_PIT_CH0, 1000 * 1); /* 设置PIT0通道 1MS定时 */
    
    while(1)
    {
        /* 读取ADC的值  kADC_MuxA是每个ADC通道的转换器 默认都是 kADC_MuxA  MuxB 一般不能用于软件触发 */
        printf("ADC:%d\r", ADC_ReadValue(HW_ADC0, kADC_MuxA));
    }
}


