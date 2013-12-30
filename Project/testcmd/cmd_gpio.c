#include "shell.h"
#include "gpio.h"
#include "common.h"

static void GPIO_ISR(uint32_t pinArray)
{
    shell_printf("GPIO ISR array:0x%x\r\n", pinArray);
	
}

int CMD_GPIO(int argc, char * const * argv)
{
    uint32_t i;
    static uint8_t init;
    if(!init)
    {
        GPIO_QuickInit(HW_GPIOA, 16, kGPIO_Mode_OPP);
        GPIO_QuickInit(HW_GPIOA,  5, kGPIO_Mode_OPP);
        GPIO_QuickInit(HW_GPIOC,  3, kGPIO_Mode_IPU);
        GPIO_QuickInit(HW_GPIOA,  4, kGPIO_Mode_IPU);
        init = 1;
    }
    if(argc == 2)
    {
        if(!strcmp(argv[1], "TOGGLE"))
        {
            GPIO_ToggleBit(HW_GPIOA, 16);
            GPIO_ToggleBit(HW_GPIOA,  5);
        }
    }
    if(argc == 2)
    {
        if(!strcmp(argv[1], "IT"))
        {
            GPIO_CallbackInstall(HW_GPIOA, GPIO_ISR);
            GPIO_CallbackInstall(HW_GPIOC, GPIO_ISR);
            GPIO_ITDMAConfig(HW_GPIOA, 4, kGPIO_IT_RisingEdge, ENABLE);
            GPIO_ITDMAConfig(HW_GPIOC, 3, kGPIO_IT_RisingEdge, ENABLE);
        }
    }
    return 0;
}

const cmd_tbl_t CommandFun_GPIO = 
{
    .name = "GPIO",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = CMD_GPIO,
    .usage = "GPIO <CMD> (CMD = TOGGLE,IT)",
    .complete = NULL,
    .help = "GPIO <TOGGLE>"
            "GPIO <IT>"
    ,
};

