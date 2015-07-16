#include "gpio.h"
#include "common.h"
#include "uart.h"


 
 
int main(void)
{
    DelayInit();
	
    GPIO_QuickInit(HW_GPIOB, 4, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 7, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOB, 4, 0);
    GPIO_WriteBit(HW_GPIOB, 5, 0);
    GPIO_WriteBit(HW_GPIOB, 6, 0);
    GPIO_WriteBit(HW_GPIOB, 7, 0);
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    
    printf("LED flash Program!\r\n");
    while(1)
    {
        
        /* LED flash */
        GPIO_ToggleBit(HW_GPIOB, 4);
        GPIO_ToggleBit(HW_GPIOB, 5);
        GPIO_ToggleBit(HW_GPIOB, 6);
        GPIO_ToggleBit(HW_GPIOB, 7);
        DelayMs(500);
    }
}


