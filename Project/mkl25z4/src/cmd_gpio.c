#include "shell.h"
#include "gpio.h"
#include "common.h"


int CMD_GPIO(int argc, char * const * argv)
{
    uint32_t i;
    if(argc == 1)
    {
        // quick init a GPIO as ouput
        GPIO_QuickInit(HW_GPIOA, 16, kGPIO_Mode_OPP);
        GPIO_QuickInit(HW_GPIOA,  5, kGPIO_Mode_OPP);
       // for(i=0;i<10;i++)
        {
            // toggle those bit
            GPIO_ToggleBit(HW_GPIOA,  5);
            GPIO_ToggleBit(HW_GPIOA, 16);
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
    .usage = "GPIO",
    .complete = NULL,
    .help = "\r\n"
};

