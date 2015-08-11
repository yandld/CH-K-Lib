#include "gpio.h"
#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "ssd1306.h"


int main(void)
{
    DelayInit();
    DelayMs(10);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOC, 2, kGPIO_Mode_OPP);

    
    printf("Hello K60!\r\n");
    

    ssd1306_init();

    while(1)
    {
        PEout(0) = !PEout(0);
        PCout(2) = !PCout(2);
        DelayMs(500);
    }
}


