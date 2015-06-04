#include "chlib_k.h"

 
int main(void)
{
    DelayInit();

    UART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
    printf("HelloWorld!\r\n");

    GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_OPP);
    while(1)
    {
        PAout(4) = !PAout(4);
    }
}




