#include "shell/shell.h"
#include "clock.h"
#include "adc.h"

static void ADC_CallBack(void)
{
    static uint32_t cnt;
    shell_printf("Enter ADC ISR:%d\r\n", cnt);
    cnt++;
}

int CMD_ADC(int argc, char * const * argv)
{
    uint8_t instance;
    shell_printf("ADC Test CMD\r\n");
    int32_t value;
    instance = ADC_QuickInit(ADC0_SE20_DM1, kADC_SingleDiff12or13);
    ADC_CallbackInstall(instance, ADC_CallBack);
    ADC_ITDMAConfig(instance, kADC_MuxA, kADC_IT_EOF, ENABLE);
    while(1)
    {
        value = ADC_QuickReadValue(ADC0_SE20_DM1);
        shell_printf("AD:%d\r", value);
        DelayMs(50);
    }
    return 0;
}






const cmd_tbl_t CommandFun_ADC = 
{
    .name = "ADC",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_ADC,
    .usage = "ADC TEST",
    .complete = NULL,
    .help = "\r\n"

};
