#include "gpio.h"
#include "common.h"
#include "systick.h"

#define LED1  PEout(6)

void SysTick_Handler(void)
{
    LED1 = !LED1;
}

int main(void)
{

    /* …Ë÷√Œ™ ‰≥ˆ */
    GPIO_QuickInit(HW_GPIOE,  6, kGPIO_Mode_OPP);
    
    DelayInit();
    
    while(1)
    {
        LED1 = !LED1;
        DelayMs(100);
    }
}


