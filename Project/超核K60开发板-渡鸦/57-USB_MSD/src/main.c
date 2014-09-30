#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "usb.h"
#include "usb_msc.h"
#include "usb_msc_scsi.h"

/* USB using Freescale USB Stack V4.1.1 */
#define LENGTH_OF_EACH_LBA              512
#define TOTAL_LOGICAL_BLOCKS_ADDRESS    64
#define DISK_SIZE                       (LENGTH_OF_EACH_LBA * TOTAL_LOGICAL_BLOCKS_ADDRESS)
uint8_t gDiskBuffer[DISK_SIZE];

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
            printf("USB_MSC_DEVICE_GET_INFO\r\n");
            device_lba_info_ptr = (PTR_DEVICE_LBA_INFO_STRUCT)val;
            device_lba_info_ptr->total_lba_device_supports = TOTAL_LOGICAL_BLOCKS_ADDRESS;	
            device_lba_info_ptr->length_of_each_lba_of_device = LENGTH_OF_EACH_LBA; 
            device_lba_info_ptr->num_lun_supported = 1;
            break;
        case USB_MSC_DEVICE_READ_REQUEST: 
            /* copy data from storage device before sending it on USB Bus 
    		   (Called before calling send_data on BULK IN endpoints)*/
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
        		USB_memcopy(gDiskBuffer + lba_data_ptr->offset,
				lba_data_ptr->buff_ptr, 
				lba_data_ptr->size);
            break;
        case USB_APP_SEND_COMPLETE:
            break;
        case USB_MSC_DEVICE_WRITE_REQUEST: 
            lba_data_ptr = (PTR_LBA_APP_STRUCT)val;
            /* read data from driver buffer to mass storage device */
            USB_memcopy(lba_data_ptr->buff_ptr, gDiskBuffer + lba_data_ptr->offset, lba_data_ptr->size);

            break;
        case USB_MSC_DEVICE_REMOVAL_REQUEST:
            
            break;
        default:
            printf("!!%X\r\n", event_type);
            break;
    }

    
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("HelloWorld\r\n");
    USB_Init();
    USB_Class_MSC_Init(0, USB_App_Callback, NULL, MSD_Event_Callback);
			
                            
    while(1)
    {
        USB_MSC_Periodic_Task(); 
    }
}


