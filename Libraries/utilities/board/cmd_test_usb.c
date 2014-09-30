#include "shell.h"
#include "gpio.h"
#include "board.h"
#include "usb_hid.h"
#include "usb.h"


void USB_App_Callback(uint8_t controller_ID, uint8_t event_type, void* val)
{
    /*
    #define USB_APP_BUS_RESET                   (0)
    #define USB_APP_CONFIG_CHANGED              (1)
    #define USB_APP_ENUM_COMPLETE               (2)
    #define USB_APP_SEND_COMPLETE               (3)
    #define USB_APP_DATA_RECEIVED               (4)
    #define USB_APP_ERROR                       (5)
    #define USB_APP_GET_DATA_BUFF               (6)
    #define USB_APP_EP_STALLED                  (7)
    #define USB_APP_EP_UNSTALLED                (8) 
    #define USB_APP_GET_TRANSFER_SIZE           (9)
    #define USB_APP_BUS_SUSPEND                 (0x0A)
    #define USB_APP_BUS_RESUME                  (0x0B)
    */
    printf("%s e:%d\r\n", __func__, event_type);
    
}
uint_8 USB_App_Param_Callback(
      uint_8 request,        /* [IN] request type */
      uint_16 value,         /* [IN] report type and ID */
      uint_16 wIndex,		 /* [IN] interface*/
      uint_8_ptr* data,      /* [OUT] pointer to the data */
      USB_PACKET_SIZE* size  /* [OUT] size of the transfer */
)
{
    printf("%s\r\n", __func__);
    return 0;
}


int DoUSB(int argc, char * const argv[])
{
    USB_Init();

    NVIC_EnableIRQ(USB0_IRQn);
    USB_Class_HID_Init(0, USB_App_Callback, NULL,
                            USB_App_Param_Callback);

    while(1);

  //  USB_Init(MAX3353);
    return 0;
}

SHELL_EXPORT_CMD(DoUSB, usb , usb test)


