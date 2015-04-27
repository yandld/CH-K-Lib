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
void usbd_msc_init ()
{
    rt_thread_t tid;
    struct rt_device_blk_geometry geometry;
    
    msd_mq = rt_mq_create("msd_mq", sizeof(msd_msg_t), 20, RT_IPC_FLAG_FIFO);

    rt_device_control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    
    USBD_MSC_MemorySize = geometry.block_size*geometry.sector_count;
    USBD_MSC_BlockSize  = geometry.block_size;
    USBD_MSC_BlockGroup = 1;
    USBD_MSC_BlockCount = geometry.sector_count;
    USBD_MSC_BlockBuf   = rt_malloc(geometry.block_size);
    USBD_MSC_MediaReady = __TRUE;
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
    int i;
    rt_thread_t tid;
    msd_msg_t msg;
    
    dev = rt_device_find((char*)parameter);
    
    if(!dev)
    {
        rt_kprintf("no %s device found!\r\n", (char*)parameter);
        tid = rt_thread_self();
        rt_thread_delete(tid);
        return ;
    }
    
    rt_device_init(dev);
    rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
    
    usbd_init();                          /* USB Device Initialization          */
    usbd_connect(__TRUE);                 /* USB Device Connect                 */

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
    rt_thread_t tid;
    char * name;
    
    if((argc != 2) || (argv[1] == RT_NULL))
    {
        return 1;
    }
    
    name = rt_malloc(rt_strlen(argv[1]));
    strcpy(name, argv[1]);
    
    tid = rt_thread_create("usb", usb_thread_entry, name, (1024*1), 0x06, 20);                                                      
    rt_thread_startup(tid);
    return 0;
}

MSH_CMD_EXPORT(udisk, udisk sd0);

