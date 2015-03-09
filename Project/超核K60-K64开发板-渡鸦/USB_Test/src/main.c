#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "usb.h"
#include "usb_hid.h"



int main(void)
{
    uint8_t buf[4];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("HelloWorld\r\n");
    if(USB_Init())
    {
        printf("USB  Init failed, clock must be 96M or 48M\r\n");
        while(1);
    }
    USB_Init2();
    NVIC_EnableIRQ(USB0_IRQn);
	
    while(1)
    {
        static int i;

    }
}


