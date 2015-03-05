#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"
#include "FlashOS.H"



int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("HelloWorld\r\n");
 
    while(1)
    {
        
    }
}


