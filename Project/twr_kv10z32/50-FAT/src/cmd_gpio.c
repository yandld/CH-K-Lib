#include "shell.h"
#include "gpio.h"

static int DoGPIO(int argc, char * const argv[])
{
    GPIO_QuickInit(HW_GPIOC, 1, kGPIO_Mode_OPP);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 1);
        DelayMs(500);
    }
}

SHELL_EXPORT_CMD(DoGPIO, gpio , gpio test)
    

