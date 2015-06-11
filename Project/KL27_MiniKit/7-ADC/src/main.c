#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"
#include "adc.h"

int cmd_adc(int argc, char * const argv[])
{
    int i;
    uint32_t val;
    ADC_QuickInit(ADC0_SE23A_PE30, kADC_SingleDiff12or13); 
    
    for(i=0;i<50;i++)
    {
        val = ADC_QuickReadValue(ADC0_SE23A_PE30);
        printf("ADC Value:%d\r\n", val);
        DelayMs(200);
    }
    return 0;
}

int main(void)
{
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    printf("7-ADC demo\r\n");
    cmd_adc(0, NULL);

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(500);
    }
}

