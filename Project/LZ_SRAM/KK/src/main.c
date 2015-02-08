#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"
#include "FlashOS.H"



int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 24, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
    
    printf("HelloWorld\r\n");
 
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 24);
        DelayMs(100);
    }
}


