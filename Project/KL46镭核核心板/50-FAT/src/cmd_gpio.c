#include "shell.h"
#include "gpio.h"

int DoGPIO(int argc, char * const argv[])
{
    int i;
    GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOD, 3, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOE, 6, 1);
    for(i=0;i<10;i++)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        GPIO_ToggleBit(HW_GPIOD, 7);
        GPIO_ToggleBit(HW_GPIOD, 3);
        DelayMs(500);
    }
}

SHELL_EXPORT_CMD(DoGPIO, gpio , gpio test)
    

