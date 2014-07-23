#include "shell.h"
#include "gpio.h"

static int DoGPIO(int argc, char * const argv[])
{
    GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 29, kGPIO_Mode_OPP);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 29);
        GPIO_ToggleBit(HW_GPIOD, 5);
        DelayMs(500);
    }
}

SHELL_EXPORT_CMD(DoGPIO, gpio , gpio test)
    

