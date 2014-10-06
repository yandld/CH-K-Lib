#include <rtthread.h>
#include <rthw.h>

#include "gpio.h"
#include "gui.h"
#include "board.h"

#include "my_gui.h"

#include "usb.h"
#include "usb_msc.h"


void USB_App_Callback(uint8_t controller_ID, uint8_t event_type, void* val)
{
    printf("USB_App_Callback%d\r\n", event_type);
    if((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED))
    {
        
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {   /* if enumeration is complete set mouse_init
           so that application can start */
        printf("USB_APP_ENUM_COMPLETE\r\n");
    }
    
}

void MSD_Event_Callback(uint_8 controller_ID, 
		uint_8 event_type, 
		void* val) 
{
    PTR_DEVICE_LBA_INFO_STRUCT device_lba_info_ptr;
    PTR_LBA_APP_STRUCT lba_data_ptr;
    switch(event_type)
	{
        case USB_MSC_DEVICE_GET_INFO:
            printf("USB_MSC_DEVICE_GET_INFO%d\r\n");
            device_lba_info_ptr = (PTR_DEVICE_LBA_INFO_STRUCT)val;
            
            device_lba_info_ptr->num_lun_supported = 1;
            break;
        case USB_MSC_DEVICE_READ_REQUEST: 
                lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
            break;
        case USB_APP_SEND_COMPLETE:
            break;
        case USB_MSC_DEVICE_WRITE_REQUEST: 
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
        case USB_MSC_DEVICE_REMOVAL_REQUEST:
            
            break;
        default:
            printf("Unknown event type:%X\r\n", event_type);
            break;
    }

    
}

void led_thread_entry(void* parameter)
{
    int i,led_num;
    
    uint32_t led_port_tab[] = BOARD_LED_GPIO_BASES;
    uint32_t led_pin_tab[] = BOARD_LED_PIN_BASES;
    led_num = ARRAY_SIZE(led_port_tab);
    for(i=0; i<led_num; i++)
    {
        GPIO_QuickInit(led_port_tab[i], led_pin_tab[i], kGPIO_Mode_OPP);
    }
    USB_Init();
    USB_Class_MSC_Init(0, USB_App_Callback, NULL, MSD_Event_Callback);
    while(1)
    {
        USB_MSC_Periodic_Task();
        for(i=0;i<led_num;i++)
        {
            GPIO_ToggleBit(led_port_tab[i], led_pin_tab[i]);
        }
        rt_thread_delay(1);
    }
}


