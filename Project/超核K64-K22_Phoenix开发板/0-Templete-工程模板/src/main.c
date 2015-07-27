#include "gpio.h"
#include "common.h"
#include "uart.h"

 
 
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("HelloWorld!\r\n");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


