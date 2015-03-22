#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"

#include <rl_usb.h>




int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    #ifdef FRDM
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    #else
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    #endif
    
    printf("USB test\r\n");
    if(USB_ClockInit())
    {
        printf("USB  Init failed, clock must be 96M or 48M\r\n");
        while(1);
    }
    
    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */

    while(1)
    {

    }
}


