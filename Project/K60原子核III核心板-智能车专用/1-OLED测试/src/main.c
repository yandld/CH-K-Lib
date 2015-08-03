#include "gpio.h"
#include "common.h"
#include "i2c.h"
#include "ssd1306.h"

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);

    GPIO_WriteBit(HW_GPIOE, 0, 0);
    
    while(1)
    {
        PEout(0) = !PEout(0);
        DelayMs(500);
    }
}


