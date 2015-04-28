#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "flash.h"
/* USB */
#include "rl_usb.h"



int main(void)
{
	uint32_t clock;
    
	DelayInit();   
    DelayMs(1);
    
    GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);    
    printf("HelloWorld\r\n");
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("kBusClock:%dHz\r\n", clock);

    /* MCU driver init */
    I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 7, 0);
    
    extern void UART_RX_ISR(uint16_t data);
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);

    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
    printf("flash demo\r\n");
    

    
    //  usbd_init();                          /* USB Device Initialization          */
    //  usbd_connect(__TRUE);                 /* USB Device Connect                 */
    // while (!usbd_configured ());          /* Wait for device to configure        */
    
    while(1)
    {
		GPIO_ToggleBit(HW_GPIOD, 5);
        DelayMs(100);
    }
}


void UART_RX_ISR(uint16_t data)
{

    printf("UART_ISR\r\n");
}
