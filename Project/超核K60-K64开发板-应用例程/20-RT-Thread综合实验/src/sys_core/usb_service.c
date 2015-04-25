#include <rtthread.h>
#include <rthw.h>
#include "rl_usb.h"

#include <stdbool.h>

typedef struct
{
    uint8_t dir;
    int cmd;
    U32 block;
    U8 *buf;
    U32 num_of_blocks;
}msd_msg_t;

static rt_device_t dev;
static rt_mq_t msd_mq;

/* init */
void usbd_msc_init ()
{
    rt_thread_t tid;
    
    msd_mq = rt_mq_create("msd_mq", sizeof(msd_msg_t), 20, RT_IPC_FLAG_FIFO);

    USBD_MSC_MemorySize = 2*1024*1024;
    USBD_MSC_BlockSize  = 4096;
    USBD_MSC_BlockGroup = 1;
    USBD_MSC_BlockCount = USBD_MSC_MemorySize / USBD_MSC_BlockSize;
    USBD_MSC_BlockBuf   = rt_malloc(4096);

    USBD_MSC_MediaReady = __TRUE;

}

void usbd_msc_read_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    static msd_msg_t msg;
    
    msg.block = block;
    msg.buf = buf;
    msg.num_of_blocks = num_of_blocks;
    msg.dir = 0;
    
   // rt_mq_send(msd_mq, &msg, sizeof(msg));
    __disable_irq();
    rt_device_read(dev, block, buf, num_of_blocks);
    __enable_irq();
}

void usbd_msc_write_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    msd_msg_t msg;
    
    msg.block = block;
    msg.buf = buf;
    msg.num_of_blocks = num_of_blocks;
    msg.dir = 1;
    
    __disable_irq();
    rt_device_write(dev, block, buf, num_of_blocks);
    __enable_irq();
}

void usb_thread_entry(void* parameter)
{
    int i;
    rt_thread_t tid;
    msd_msg_t msg;
    
    dev = rt_device_find("sf0");
    if(!dev)
    {
        rt_kprintf("no device found!\r\n");
        tid = rt_thread_self();
        rt_thread_delete(tid); 
    }
    
    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */
    while (!usbd_configured ())
    {
        rt_thread_delay(10);
    }
    
    rt_kprintf("usb enum complete\r\n");
    
    while(1)
    {
        if(rt_mq_recv(msd_mq, &msg, sizeof(msd_msg_t), RT_WAITING_FOREVER) == RT_EOK)
        {
            if(msg.dir == 0)
            {
          //      rt_device_read(dev, msg.block, msg.buf, msg.num_of_blocks);
            }
            else
            {
          //      rt_device_write(dev, msg.block, msg.buf, msg.num_of_blocks);
            }
        }
    }
}

