#include <stdio.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"


int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);

    printf("HelloWorld!\r\n");
    
    uint8_t ch;

    while(1)
    {
        putchar(getchar());
    }
}


