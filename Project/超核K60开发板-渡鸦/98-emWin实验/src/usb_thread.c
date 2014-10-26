#include <rtthread.h>
#include <rthw.h>

#include "gpio.h"
#include "gui.h"
#include "board.h"

#include "my_gui.h"

#include "usb.h"
#include "usb_msc.h"

static rt_device_t device = RT_NULL;
    

static void USB_App_Callback(uint8_t controller_ID, uint8_t event_type, void* val)
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

static void MSD_Event_Callback(uint_8 controller_ID, 
		uint_8 event_type, 
		void* val) 
{
    PTR_DEVICE_LBA_INFO_STRUCT device_lba_info_ptr;
    PTR_LBA_APP_STRUCT lba_data_ptr;
    struct rt_device_blk_geometry geometry;
    switch(event_type)
	{
        case USB_MSC_DEVICE_GET_INFO:
            device_lba_info_ptr = (PTR_DEVICE_LBA_INFO_STRUCT)val;
            rt_device_control(device, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
            device_lba_info_ptr->total_lba_device_supports = geometry.sector_count;	
            device_lba_info_ptr->length_of_each_lba_of_device = 512;
            device_lba_info_ptr->num_lun_supported = 1;
            break;
        case USB_MSC_DEVICE_READ_REQUEST: 
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
         //   rt_device_read(device, lba_data_ptr->offset/512, lba_data_ptr->buff_ptr, 1);
        //    SD_ReadSingleBlock(lba_data_ptr->offset/LENGTH_OF_EACH_LBA, lba_data_ptr->buff_ptr);
            break;
        case USB_APP_SEND_COMPLETE:
            break;
        case USB_MSC_DEVICE_WRITE_REQUEST: 
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
          //  rt_device_write(device, lba_data_ptr->offset/512, lba_data_ptr->buff_ptr, 1);
          //  DisableInterrupts;
         //   SD_WriteSingleBlock(lba_data_ptr->offset/LENGTH_OF_EACH_LBA, lba_data_ptr->buff_ptr);
        //    EnableInterrupts;
            break;
        case USB_MSC_DEVICE_REMOVAL_REQUEST:
            
            break;
        default:
            printf("Unknown event type:%X\r\n", event_type);
            break;
    }

    
}

void usb_thread_entry(void* parameter)
{
    int i;

    device = rt_device_find((char*)parameter);
    if( device == RT_NULL)
    {
        rt_kprintf("%s:device %s: not found!\r\n", __func__, parameter);
        return;
    }
    rt_device_init(device);
    rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    USB_Init();
    DisableInterrupts;
    USB_Class_MSC_Init(0, USB_App_Callback, NULL, MSD_Event_Callback);
    EnableInterrupts;
    while(1)
    {
        USB_MSC_Periodic_Task();
        rt_thread_delay(1);
    }
}


