#include <stdio.h>
#include "adc.h"
#include "common.h"
#include "lpuart.h"

int cmd_adc(int argc, char * const argv[])
{
    int i;
    uint32_t val;
    ADC_QuickInit(ADC0_SE23A_PE30, kADC_SingleDIff16); 
    
    
    for(i=0;i<10;i++)
    {
        val = ADC_QuickReadValue(ADC0_SE23A_PE30);
        printf("ADC0_SE23A_PE30:%d\r\n", val);
        DelayMs(200);
    }
    return 0;
}

