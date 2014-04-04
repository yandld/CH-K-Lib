#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dac.h"

int main(void)
{
    uint32_t i;
    uint16_t value[16];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DAC test please measure DAC0_OUT\r\n");
    
    DAC_InitTypeDef DAC_InitStruct = {0};
    DAC_InitStruct.bufferMode = kDAC_Buffer_Swing; /*Buffer摇摆模式 产生三角波 */
    DAC_InitStruct.instance = HW_DAC0;
    DAC_InitStruct.referenceMode = kDAC_Reference_2; /* 使用VDDA作为参考源 */
    DAC_InitStruct.triggerMode = kDAC_TriggerSoftware; /*软件触发 */
    DAC_Init(&DAC_InitStruct);
    for(i = 0; i < 16; i++)
    {
        value[i] = i*4096/16;
    }
    DAC_SetBufferValue(HW_DAC0, value, 16); /*写入DAC buffer 最多写入16个 uint16_t 的转换值 */
    while(1)
    {
        /* 不停的触发 DAC 来产生波形 */
        DAC_StartConversion(HW_DAC0);
    }
}


