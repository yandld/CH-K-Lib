/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************//*!
*
* @file usb_printer.c
*
* @author
*
* @version
*
* @date
*
* @brief The file contains USB stack printer layer implementation.
*
*****************************************************************************/
/******************************************************************************
* Includes
*****************************************************************************/
#include "usb_printer.h"    /* USB printer class header file */
#include "usb_devapi.h"     /* USB device header file */

/****************************************************************************
* Local Variables
****************************************************************************/
/* PRINTER endpoint info array */
static USB_CLASS_PRINTER_ENDPOINT g_printer_ep[PRINTER_DESC_ENDPOINT_COUNT];
/* PRINTER Class Callback Function Pointer */
static USB_CLASS_CALLBACK g_printer_class_callback=NULL;
/* PRINTER Class Vendor Callback Function Pointer */
static USB_REQ_FUNC       g_vendor_req_callback=NULL;
/* PRINTER Class Param Callback Function Pointer */
static USB_CLASS_SPECIFIC_HANDLER_FUNC g_param_callback = NULL;

static uint_32 g_current_offset = 0;
static uint_32 g_transfer_remaining = 0;
static uint_8 g_class_port_status = 0;
static uint_8 printer_send_buff[PRINTER_SEND_DATA_BUFF_SIZE];
/*****************************************************************************
* Local Types - None
*****************************************************************************/
/*****************************************************************************
* Local Functions Prototypes
*****************************************************************************/
static uint_8 USB_Other_Requests(
    uint_8 controller_ID,
    USB_SETUP_STRUCT * setup_packet,
    uint_8_ptr *data,
    USB_PACKET_SIZE *size
);
static uint_8 USB_Map_Ep_To_Struct_Index (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 ep_num           /* Endpoint Number */
);
/*****************************************************************************
* Local Variables - None
*****************************************************************************/

/*****************************************************************************
* Local Functions
*****************************************************************************/
 /**************************************************************************//*!
 *
 * @name  USB_Map_Ep_To_Struct_Index
 *
 * @brief The funtion maps the endpoint number to the index of the endpoint
 *        data structure
 *
 * @param controller_ID     : Controller ID
 * @param ep_num            : Endpoint number
 *
 * @return index            : mapped index
 *
 ******************************************************************************
 * Returns the index of the endpoint info array for the particular endpoint
 * number
 *****************************************************************************/
static uint_8 USB_Map_Ep_To_Struct_Index (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 ep_num           /* Endpoint Number */
)
{
    uint_8 index = 0;

    USB_ENDPOINTS *ep_desc_data = (USB_ENDPOINTS *)
        USB_Desc_Get_Endpoints(controller_ID);
    /* map the endpoint num to the index of the endpoint structure */
    for(index = 0; index < ep_desc_data->count; index++)
    {
        if(ep_desc_data->ep[index].ep_num == ep_num)
        {
            break;
        }
    }
    return index;
}
/**************************************************************************//*!
*
* @name  USB_Printer_Service_Bulk_In
*
* @brief The funtion ic callback function of Printer Bulk In Endpoint 
*
* @param event
*
* @return None
*
*****************************************************************************/
void USB_Printer_Service_Bulk_In(PTR_USB_DEV_EVENT_STRUCT event)
{
    uint_8 event_type; 
    #if IMPLEMENT_QUEUING
    uint_8 index;
    uint_8 producer, consumer;
    USB_ENDPOINTS *usb_ep_data = USB_Desc_Get_Endpoints(event->controller_ID); 
    USB_CLASS_PRINTER_QUEUE queue;
    /* map the endpoint num to the index of the endpoint structure */
    for(index = 0; index < usb_ep_data->count; index++) 
    {
        if(usb_ep_data->ep[index].ep_num == event->ep_num)
        break;
    }
    producer = g_printer_ep[index].bin_producer;
    /* if there are no errors de-queue the queue and decrement the no. of 
       transfers left, else send the same data again */
    g_printer_ep[index].bin_consumer++;
    consumer = g_printer_ep[index].bin_consumer;
    if(consumer != producer) 
    {/*if bin is not empty */
        queue = g_printer_ep[index].queue[consumer%MAX_QUEUE_ELEMS];
        g_transfer_remaining = queue.size;
        g_current_offset = 0;
        (void)USB_Class_Send_Data(queue.controller_ID, queue.channel, 
            queue.app_buff, queue.size);
        return;
    }
    #endif
    if(g_transfer_remaining >= event->len)
    {
        /* decrement the global count */
        g_transfer_remaining -= event->len;
    }
    
    if(g_printer_class_callback != NULL)
    {
        if(event->errors != 0){
        /* notify the application of the error */
        g_printer_class_callback(event->controller_ID, USB_APP_ERROR,
            (uint_8*)(&(event->errors)));
        }
        else
        {
            event_type = (uint_8)(g_transfer_remaining?
                USB_APP_GET_DATA_BUFF : USB_APP_SEND_COMPLETE);
            if(event_type == USB_APP_SEND_COMPLETE)
            {
                APP_PRINTER_DATA_STRUCT printer_app;
                printer_app.data_ptr = event->buffer_ptr;
                printer_app.data_size = event->len; 
                g_printer_class_callback(event->controller_ID, event_type, (void*)&printer_app);
            }
            else
            {
                PRINTER_APP_STRUCT printer_data;
                g_current_offset += event->len;
                printer_data.offset = g_current_offset;
                printer_data.size = (g_transfer_remaining > PRINTER_SEND_DATA_BUFF_SIZE) ? 
                    PRINTER_SEND_DATA_BUFF_SIZE : g_transfer_remaining; 
                /* whichever is smaller */
                printer_data.buff_ptr = printer_send_buff;
                /* Get data from application */
                g_printer_class_callback(event->controller_ID, USB_APP_GET_DATA_BUFF,
                    (void*)&printer_data);
                (void)USB_Class_Printer_Send_Data(event->controller_ID, event->ep_num, printer_data.buff_ptr, 
                    printer_data.size);
            }
        }
    }
}

/**************************************************************************//*!
*
* @name  USB_Printer_Service_Bulk_Out
*
* @brief The function is callback function of Printer Bulk Out Endpoint 
*
* @param event
*
* @return None
*
*****************************************************************************/
void USB_Printer_Service_Bulk_Out(PTR_USB_DEV_EVENT_STRUCT event)
{ 
    if(g_printer_class_callback != NULL)
    {
        if(event->errors != 0)
        {
            g_printer_class_callback(event->controller_ID, USB_APP_ERROR,
                    (uint_8*)(&(event->errors)));
        }  
        else 
        {
            g_printer_class_callback(event->controller_ID, USB_APP_DATA_RECEIVED,
                (uint_8*)(event->buffer_ptr));
        }
    }
}

/**************************************************************************//*!
*
* @name  USB_Class_Printer_Event
*
* @brief The funtion initializes printer endpoints 
*
* @param controller_ID:  To identify the controller   
* @param event           pointer to event structure
* @param val             gives the configuration value 
*
* @return None       
*
*****************************************************************************/
void USB_Class_Printer_Event(uint_8 controller_ID, uint_8 event, void* val) 
{    
#if IMPLEMENT_QUEUING    
    uint_8 index;
#endif    
    /* get the endpoints from the descriptor module */            
    USB_ENDPOINTS *usb_ep_data = USB_Desc_Get_Endpoints(controller_ID); 
    
    if(event == USB_APP_ENUM_COMPLETE)
    {
        uint_8 index_num = 0;
        uint_8 count = 0,ep_count = 0;                               
        ep_count = usb_ep_data->count; 
        /* deinitialize all endpoints in case they were initialized */
        for(count=index_num; count<ep_count+index_num; count++) 
        {   
            USB_EP_STRUCT_PTR ep_struct_ptr= 
            (USB_EP_STRUCT_PTR) (&usb_ep_data->ep[count]);
            (void)_usb_device_deinit_endpoint(&controller_ID,
            ep_struct_ptr->ep_num, ep_struct_ptr->direction);
        }

        /* intialize all non control endpoints */            
        for(count=index_num; count<ep_count+index_num; count++) 
        {
            USB_EP_STRUCT_PTR ep_struct_ptr= 
            (USB_EP_STRUCT_PTR) (&usb_ep_data->ep[count]);
            uint_8 component = (uint_8)(ep_struct_ptr->ep_num | 
            (ep_struct_ptr->direction<<COMPONENT_PREPARE_SHIFT));           

            (void)_usb_device_init_endpoint(&controller_ID,ep_struct_ptr->ep_num,
            (uint_16)ep_struct_ptr->size, ep_struct_ptr->direction, ep_struct_ptr->type, FALSE);

#ifdef MULTIPLE_DEVICES
            /* register callback service for Non Control EndPoints */
            if(ep_struct_ptr->type == USB_BULK_PIPE) 
            {             
                if(ep_struct_ptr->direction == USB_RECV) 
                {
                    (void)_usb_device_register_service(controller_ID,
                    (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                    USB_Printer_Service_Bulk_Out);
                }
                else
                {
                    (void)_usb_device_register_service(controller_ID,
                    (uint_8)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                    USB_Printer_Service_Bulk_In);
                }            
            }
#endif
            /* set the EndPoint Status as Idle in the device layer */
            /* (no need to specify direction for this case) */
            (void)_usb_device_set_status(&controller_ID,
                (uint_8)(USB_STATUS_ENDPOINT|component),
                (uint_16)USB_STATUS_IDLE);                                                                                                         
        }
    }
    else if(event == USB_APP_BUS_RESET)
    {
#if IMPLEMENT_QUEUING
        for(index = 0; index < usb_ep_data->count; index++) 
        {
            g_printer_ep[index].bin_consumer = 0x00;         
            g_printer_ep[index].bin_producer = 0x00;        
        }
#endif 
    }
    else if(event == USB_APP_EP_UNSTALLED)
    {
        uint_8 value;
        value = *((uint_8_ptr)val);     
    }
    else if(event == USB_APP_EP_STALLED)
    {
        /* Code to be added here, 
            if there is some action needed at app level */            
    }

    if(g_printer_class_callback != NULL)
    {
        g_printer_class_callback(controller_ID, event, val);
    }

}

/**************************************************************************//*!
*
* @name  USB_Other_Requests
*
* @brief   The funtion provides flexibilty to add class and vendor specific
*              requests
*
* @param controller_ID     : Controller ID
* @param setup_packet      : Setup packet received
* @param data              : Data to be send back
* @param size              : Size to be returned
*
* @return status:
*             USB_OK        : When Successfull
*             Others        : When Error
*
******************************************************************************
* Handles Printer Class requests and forwards vendor specific request to the
* application
*****************************************************************************/
uint_8 USB_Other_Requests(
    uint_8 controller_ID,               /* [IN] Controller ID */
    USB_SETUP_STRUCT * setup_packet,    /*[IN] Setup packet received */
    uint_8_ptr *data,                   /* [OUT] Data to be send back */
    USB_PACKET_SIZE *size               /* [OUT] Size to be returned*/
)
{
    uint_8 error = USBERR_INVALID_REQ_TYPE; 
    uint_8 string_id_buff[STRING_ID_SIZE];/* buffer to send in case of get report req */
    *((uint_32_ptr)string_id_buff) = 0;
    
    *size=0;
    if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_CLASS)
    {
        switch(setup_packet->request)
        {
            case USB_PRINTER_GET_DEVICE_ID:
                *data = &string_id_buff[0];
                *size = STRING_ID_SIZE;
                break;
            case USB_PRINTER_GET_PORT_STATUS :
                *data = &g_class_port_status;
                *size = 1;
                break;
            case USB_PRINTER_SOFT_RESET :
                break;
        }
        if(g_param_callback != NULL)
        {
            /* 
               handle callback if the application has supplied it
               set the size of the transfer from the setup packet 
            */
            *size = (USB_PACKET_SIZE)setup_packet->length; 

            /* notify the application of the class request.
               give control to the application */
            error = g_param_callback(setup_packet->request,/* request type */
                setup_packet->value,
                0,
                data,/* pointer to the data */
                size);/* size of the transfer */
        }
    } 
    else if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) == 
        USB_REQUEST_CLASS_VENDOR) 
    {   /* vendor specific request  */    
        if(g_vendor_req_callback != NULL)
        {
            error = g_vendor_req_callback(controller_ID, setup_packet, data,size);
        }
    }
    return error;
}

/*****************************************************************************
* Global Functions
*****************************************************************************/

/**************************************************************************//*!
*
* @name  USB_Class_Printer_Init
*
* @brief   The funtion initializes the Device and Controller layer
*
* @param   controller_ID               : Controller ID
* @param   printer_class_callback      : Printer Class Callback
* @param   vendor_req_callback         : Vendor Request Callback
* @param   param_callback              : Class requests Callback
*
* @return status:
*                        USB_OK        : When Successfull
*                        Others        : When Error
*
******************************************************************************
*This function initializes the PRINTER Class layer and layers it is dependent on
*****************************************************************************/
uint_8 USB_Class_Printer_Init (
    uint_8             controller_ID,                /* [IN] Controller ID */
    USB_CLASS_CALLBACK printer_class_callback,       /* [IN] PRINTER Class Callback */
    USB_REQ_FUNC       vendor_req_callback,          /* [IN] Vendor Request Callback */
    USB_CLASS_SPECIFIC_HANDLER_FUNC param_callback   /* [ IN] PRINTER Class requests Callback */
)
{
    #if IMPLEMENT_QUEUING    
    uint_8 index;
    #endif    
    uint_8 error;   
    USB_ENDPOINTS *usb_ep_data = USB_Desc_Get_Endpoints(controller_ID); 
    

#ifndef COMPOSITE_DEV        
    /* Initialize the device layer*/
    error= _usb_device_init(controller_ID, NULL, (uint_8)(usb_ep_data->count+1), TRUE);
    /* +1 is for Control Endpoint */
    
    /* Initialize number of Logical Units */
    if(error == USB_OK) 
    {
        /* Initialize the generic class functions */
        error = USB_Class_Init(controller_ID,USB_Class_Printer_Event,
        USB_Other_Requests);
#endif
        #if IMPLEMENT_QUEUING
        for(index = 0; index < usb_ep_data->count; index++) 
        {
            g_printer_ep[index].endpoint = usb_ep_data->ep[index].ep_num;
            g_printer_ep[index].type = usb_ep_data->ep[index].type;        
            g_printer_ep[index].bin_consumer = 0x00;         
            g_printer_ep[index].bin_producer = 0x00;        
        }
        #endif
        
        if(error == USB_OK) 
        {
            /* save the callback pointer */
            g_printer_class_callback = printer_class_callback;

            /* save the callback pointer */
            g_vendor_req_callback = vendor_req_callback;
            /* save the callback pointer */
            g_param_callback = param_callback;           
        }
#ifndef COMPOSITE_DEV                
    }
#endif    
    return error;
}

/**************************************************************************//*!
*
* @name  USB_Class_Printer_DeInit
*
* @brief   The funtion de-initializes the Device and Controller layer
*
* @param   controller_ID               : Controller ID
*
* @return status:
*                        USB_OK        : When Successfull
*                        Others        : When Error
*
******************************************************************************
*This function de-initializes the Printer Class layer
*****************************************************************************/
uint_8 USB_Class_Printer_DeInit 
(
    uint_8 controller_ID              /* [IN] Controller ID */
) 
{
    uint_8 status = USB_OK;
#ifdef COMPOSITE_DEV 
    UNUSED(controller_ID)
#endif  
    /* free the PRINTER class callback pointer */
    g_printer_class_callback = NULL;
    
    /* free the vendor request callback pointer */
    g_vendor_req_callback = NULL;
    
    /* free the callback to ask application for class specific params*/
    g_param_callback = NULL;
    
#ifndef COMPOSITE_DEV
    /* Call common class deinit function */
    status = USB_Class_DeInit(controller_ID);
    
    if(status == USB_OK)
    /* Call device deinit function */
    status = _usb_device_deinit();
#endif    
    return status;
}

/**************************************************************************//*!
*
* @name  USB_Class_Printer_Send_Data
*
* @brief This fucntion is used by Application to send data through Printer class
*
* @param controller_ID     : Controller ID
* @param ep_num            : Endpoint number
* @param app_buff          : Buffer to send
* @param size              : Length of the transfer
*
* @return status:
*                        USB_OK        : When Successfull
*                        Others        : When Error
*
******************************************************************************
* This function is used by Application to send data through Printer class
*****************************************************************************/
uint_8 USB_Class_Printer_Send_Data (
    uint_8 controller_ID,       /* [IN] Controller ID */
    uint_8 ep_num,              /* [IN] Endpoint Number */
    uint_8_ptr app_buff,        /* [IN] Buffer to Send */
    USB_PACKET_SIZE size        /* [IN] Length of the Transfer */
)
{
    uint_8 index;
    volatile uint_8 producer, consumer;
    uint_8 status = USB_OK;

    USB_ENDPOINTS *ep_desc_data = (USB_ENDPOINTS *)
    USB_Desc_Get_Endpoints(controller_ID);

    /* map the endpoint num to the index of the endpoint structure */
    index = USB_Map_Ep_To_Struct_Index(controller_ID, ep_num);

    producer = g_printer_ep[index].bin_producer;
    consumer = g_printer_ep[index].bin_consumer;

    if((uint_8)(producer - consumer) != (uint_8)(MAX_QUEUE_ELEMS))
    {
        /* the bin is not full*/

        uint_8 queue_num = (uint_8)(producer % MAX_QUEUE_ELEMS);

        /* queue the send request */
        /* put all send request parameters in the endpoint data structure */
        g_printer_ep[index].queue[queue_num].controller_ID =  controller_ID;
        g_printer_ep[index].queue[queue_num].channel = ep_num;
        g_printer_ep[index].queue[queue_num].app_buff = app_buff;
        g_printer_ep[index].queue[queue_num].size = size;

        /* increment producer bin by 1*/
        g_printer_ep[index].bin_producer++;
        producer++;

        if((uint_8)(producer - consumer) == (uint_8)1)
        {
            /* 
                This is required for Bridges that send APDU fragmented
            */
            g_transfer_remaining = size;
            g_current_offset = 0;
            /* send the IO if there is only one element in the queue */
            status = USB_Class_Send_Data(controller_ID, ep_num, app_buff,size);
        }
    }
    else /* bin is full */
    {
        status = USBERR_DEVICE_BUSY;
    }
    return status;
}
