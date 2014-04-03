#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dac.h"
#include <math.h>

int main(void)
{
    uint32_t i,j;
    float ii;
    uint16_t sin_value[16];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DAC test...\r\n");
    
    DAC_InitTypeDef DAC_InitStruct = {0};
    
    DAC_InitStruct.bufferMode = kDAC_Buffer_Swing; /*Buffer摇摆模式 产生三角波 */
    DAC_InitStruct.instance = HW_DAC0;
    DAC_InitStruct.referenceMode = kDAC_Reference_2;
    DAC_InitStruct.triggerMode = kDAC_TriggerSoftware;
    DAC_Init(&DAC_InitStruct);
    for(i = 0; i < 16; i++)
    {
        sin_value[i] = i*4096/16;
    }
    DAC_SetBufferValue(HW_DAC0, sin_value, 16);
    while(1)
    {
        DAC_StartConversion(HW_DAC0);
    }
}


