#include <rtthread.h>
#include <rthw.h>
#include "rl_usb.h"

#include <stdbool.h>

typedef struct
{
    uint8_t flag;
    uint8_t ep;
    void (*exec)(uint8_t flag);
}msd_msg_t;

static rt_device_t dev;

rt_mq_t msd_mq;

/* init */
void usbd_msc_init (void)
{

}

void inline usbd_msc_read_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    rt_device_read(dev, block, buf, num_of_blocks);
}

void inline usbd_msc_write_sect (U32 block, U8 *buf, U32 num_of_blocks)
{
    rt_device_write(dev, block, buf, num_of_blocks);
}

void usb_thread_entry(void* parameter)
{
    msd_msg_t msg;
    
    usbd_init();
    msd_mq = rt_mq_create("msd_mq", sizeof(msd_msg_t), 20, RT_IPC_FLAG_FIFO);
    while(1)
    {
        if(rt_mq_recv(msd_mq, &msg, sizeof(msd_msg_t), RT_WAITING_FOREVER) == RT_EOK)
        {
            msg.exec(msg.flag);
        }
    }
}

#include "finsh.h"
static int udisk(int argc, char** argv)
{
    rt_err_t err;
    
    if((argc != 2) || (argv[1] == RT_NULL))
    {
        return 1;
    }
    
    dev = rt_device_find((char*)argv[1]);

    if((!dev) || (dev->type != RT_Device_Class_Block))
    {
        rt_kprintf("bad device:%s\r\n", (char*)argv[1]);
        return 1;
    }
    
    err = 0;
    err += rt_device_init(dev);
    err += rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
    
    if(err && (err != -RT_EBUSY))
    {
        rt_kprintf("init device failed:%d\r\n", err);
        return 1;
    }
    
    struct rt_device_blk_geometry geometry;
    rt_device_control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    
    USBD_MSC_MemorySize = geometry.block_size*geometry.sector_count;
    USBD_MSC_BlockSize  = geometry.block_size;
    USBD_MSC_BlockGroup = 8;
    USBD_MSC_BlockCount = geometry.sector_count;
    USBD_MSC_BlockBuf   = rt_malloc(geometry.block_size*8);
    USBD_MSC_MediaReady = __TRUE;
    
    
    if(usbd_configured())
    {
        usbd_connect(__FALSE);
        rt_thread_delay(1);
    }
    
    usbd_reset_core();
    usbd_connect(__TRUE);
    
    return 0;
}

MSH_CMD_EXPORT(udisk, udisk sd0);

