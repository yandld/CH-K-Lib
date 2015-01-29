#include <stdio.h>

#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "usb.h"
#include "usb_cdc.h"

#define DATA_BUFF_SIZE     (64)

/* Virtual COM Application start Init Flag */
static volatile boolean start_app = FALSE;
/* Virtual COM Application Carrier Activate Flag */
//static volatile boolean start_transactions = FALSE;
static volatile boolean start_transactions = TRUE;
/* Receive Buffer */
static uint_8 g_curr_recv_buf[DATA_BUFF_SIZE];
/* Send Buffer */
static uint_8 g_curr_send_buf[DATA_BUFF_SIZE];
/* Receive Data Size */
static uint_8 g_recv_size;
/* Send Data Size */
static uint_8 g_send_size;

/* USB using Freescale USB Stack V4.1.1 */
static void USB_Notify_Callback (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 event_type,      /* [IN] PSTN Event Type */
    void* val               /* [IN] gives the configuration value */
)
{
    printf("need request:%d\r\n", event_type);
    if(event_type == USB_APP_CDC_CARRIER_ACTIVATED)
    {
            start_transactions = TRUE;
    }
}


static void Virtual_Com_App(void)
{
    static uint_8 status = 0;
    /* Loopback Application Code */
    if(g_recv_size)
    {
    	/* Copy Received Buffer to Send Buffer */
    	for (status = 0; status < g_recv_size; status++)
    	{
    		g_curr_send_buf[status] = g_curr_recv_buf[status];
    	}
    	g_send_size = g_recv_size;
    	g_recv_size = 0;
    }
    if(g_send_size)
    {
        /* Send Data to USB Host*/
        uint_8 size = g_send_size;
        g_send_size = 0;
        status = USB_Class_CDC_Interface_DIC_Send_Data(0,
        g_curr_send_buf,size);
        if(status != USB_OK)
        {
            /* Send Data Error Handling Code goes here */
        }
    }

    return;
}

static void USB_App_Callback (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 event_type,      /* [IN] value of the event */
    void* val               /* [IN] gives the configuration value */
)
{

    if(event_type == USB_APP_BUS_RESET)
    {
        start_app=FALSE;
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {
        printf("USB_APP_ENUM_COMPLETE\r\n");
        start_app=TRUE;
    }
    else if((event_type == USB_APP_DATA_RECEIVED)&&
            (start_transactions == TRUE))
    {
        /* Copy Received Data buffer to Application Buffer */
        USB_PACKET_SIZE BytesToBeCopied;
        APP_DATA_STRUCT* dp_rcv = (APP_DATA_STRUCT*)val;

        BytesToBeCopied = (USB_PACKET_SIZE)((dp_rcv->data_size > DATA_BUFF_SIZE) ?
                                      DATA_BUFF_SIZE:dp_rcv->data_size);
        memcpy(g_curr_recv_buf, dp_rcv->data_ptr, BytesToBeCopied);
        g_recv_size = BytesToBeCopied;
        Virtual_Com_App();
    }
    else if((event_type == USB_APP_SEND_COMPLETE) && (start_transactions == TRUE))
    {
        /* Previous Send is complete. Queue next receive */
        (void)USB_Class_CDC_Interface_DIC_Recv_Data(0, NULL, 0);
    }

    return;
}




int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("USB CDC Test\r\n");
    if(USB_Init())
    {
        printf("USB  Init failed, clock must be 96M or 48M\r\n");
        while(1);
    }
    USB_Class_CDC_Init(0, USB_App_Callback, NULL, USB_Notify_Callback, TRUE);
    NVIC_EnableIRQ(USB0_IRQn);
	
    while(1)
    {
        /* call the periodic task function */
        USB_Class_CDC_Periodic_Task();
    }
}


