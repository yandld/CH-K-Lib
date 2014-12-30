#include <rtthread.h>
#include <rthw.h>

#include "gpio.h"
#include "gui.h"
#include "board.h"


#include "usb.h"
#include "usb_msc.h"


static rt_device_t device = RT_NULL;
    

static void USB_App_Callback(uint8_t controller_ID, uint8_t event_type, void* val)
{
    if((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED))
    {
        
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {
        rt_kprintf("Usb App enum OK.\r\n");
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
            rt_kprintf("USB_MSC_DEVICE_GET_INFO\r\n");
            device_lba_info_ptr->total_lba_device_supports = geometry.sector_count;	
            device_lba_info_ptr->length_of_each_lba_of_device = 512;
            device_lba_info_ptr->num_lun_supported = 1;
            break;
        case USB_MSC_DEVICE_READ_REQUEST: 
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
            rt_device_read(device, lba_data_ptr->offset/512, lba_data_ptr->buff_ptr, 1);
            //SD_ReadSingleBlock(lba_data_ptr->offset/512, lba_data_ptr->buff_ptr);
            break;
        case USB_APP_SEND_COMPLETE:
            break;
        case USB_MSC_DEVICE_WRITE_REQUEST: 
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
            rt_device_write(device, lba_data_ptr->offset/512, lba_data_ptr->buff_ptr, 1);
          //  SD_WriteSingleBlock(lba_data_ptr->offset/512, lba_data_ptr->buff_ptr);
            break;
        case USB_MSC_DEVICE_REMOVAL_REQUEST:
            
            break;
        default:
            rt_kprintf("Unknown event type:%X\r\n", event_type);
            break;
    }
}



void usb_thread_entry(void* parameter)
{
    int i;
    
    device = rt_device_find("sd0");
    
    if( device == RT_NULL)
    {
        rt_kprintf("%s:device %s: not found!\r\n", __func__, parameter);
        return;
    }
    
    rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    if(USB_Init())
    {
        rt_kprintf("USB  Init failed, clock must be 96M or 48M\r\n");
        return;
    }
    
    USB_Class_MSC_Init(0, USB_App_Callback, NULL, MSD_Event_Callback);
    NVIC_EnableIRQ(USB0_IRQn);
    
    while(1)
    {
        USB_MSC_Periodic_Task();
        rt_thread_delay(1);
    }
}


#ifdef FINSH_USING_MSH
#include "finsh.h"

int usb(int argc, char** argv)
{
    rt_thread_t tid;
    
    /* this task can not be single */
    tid = rt_thread_find("usb_exe");
    if(tid != RT_NULL) return -1;
    
    tid = rt_thread_create("usb_exe", usb_thread_entry, RT_NULL, 1024*2, 30, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    return 0;
}

MSH_CMD_EXPORT(usb, show a picture file.);

#endif

