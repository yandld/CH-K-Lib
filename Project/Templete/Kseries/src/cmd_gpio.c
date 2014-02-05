#include "shell/shell.h"
#include "gpio.h"


int DoGPIO(int argc, char *const argv[])
{
    uint8_t i;

    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOD, 0, kGPIO_Mode_OPP);
 
    for(i=0;i<10;i++)
    {
        GPIO_ToggleBit(HW_GPIOA, 1);
        GPIO_ToggleBit(HW_GPIOD, 0); 
        DelayMs(100);
    }
    return 0;
}



const cmd_tbl_t CommandFun_GPIO = 
{
    .name = "GPIO",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoGPIO,
    .usage = "GPIO",
    .complete = NULL,
    .help = "\r\n"
                "GPIO clock - print CPU clock\r\n"
                "GPIO memory   - print CPU memory info"
};

