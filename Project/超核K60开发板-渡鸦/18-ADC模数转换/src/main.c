#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"



int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* 为了代码简单 这里直接调用快速初始化了 */
    
    printf("ADC test\r\n");
    
    /* 初始化ADC模块 */
    ADC_InitTypeDef AD_InitStruct1;
    
    AD_InitStruct1.instance = HW_ADC0;
    AD_InitStruct1.chl = 19;
    AD_InitStruct1.clockDiv = kADC_ClockDiv2;
    AD_InitStruct1.resolutionMode = kADC_SingleDiff10or11; /*单端 10位精度 查分 11位精度 */
    AD_InitStruct1.triggerMode = kADC_TriggerSoftware;
    AD_InitStruct1.singleOrDiffMode = kADC_Single; /*单端模式 */
    AD_InitStruct1.continueMode = kADC_ContinueConversionDisable; /*禁止连续 转换 */
    AD_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable; /*禁止 硬件平均 功能 */
    
    ADC0_SE19_DM0
    while(1)
    {
        /* 等待中断 */
    }
}


