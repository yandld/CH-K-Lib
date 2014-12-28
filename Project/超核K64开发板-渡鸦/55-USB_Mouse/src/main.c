#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "usb.h"
#include "usb_hid.h"

/* USB using Freescale USB Stack V4.1.1 */

void USB_App_Callback(uint8_t controller_ID, uint8_t event_type, void* val)
{
    if((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED))
    {
        
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {   /* if enumeration is complete set mouse_init
           so that application can start */
        printf("USB_APP_ENUM_COMPLETE\r\n");
    }
    
}
uint_8 USB_App_Param_Callback(
      uint_8 request,        /* [IN] request type */
      uint_16 value,         /* [IN] report type and ID */
      uint_16 wIndex,		 /* [IN] interface*/
      uint_8_ptr* data,      /* [OUT] pointer to the data */
      USB_PACKET_SIZE* size  /* [OUT] size of the transfer */
)
{
    
    printf("need request:%d\r\n", request);
    return 0;
}


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
    USB_Class_HID_Init(0, USB_App_Callback, NULL, USB_App_Param_Callback);
    NVIC_EnableIRQ(USB0_IRQn);
	
    while(1)
    {
        static int i;
        USB_Class_HID_Periodic_Task();
        
        /* send USB data */
        i++;i%=4;
        switch(i)
        {
            case 0:
                buf[1] = 2; buf[2] = 0;
                break;
            case 1:
                buf[1] = 0; buf[2] = 2; 
                break;
            case 2:
                buf[1] = -2; buf[2] = 0;
                break;
            case 3:
                buf[1] = 0; buf[2] = -2;  
                break;
        }
        DelayMs(50);
        USB_Class_HID_Send_Data(0, HID_ENDPOINT, buf, 4);
    }
}


