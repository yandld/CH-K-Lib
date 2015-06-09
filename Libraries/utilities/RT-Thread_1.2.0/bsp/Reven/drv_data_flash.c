#include <rthw.h>
#include <rtthread.h>
#include "flash.h"
#include "rtt_drv.h"

#if defined(MK60D10)
#define FLASH_SIZE          (512*1024)
#elif defined(MK64F12)
#define FLASH_SIZE          (1024*1024)
#endif

extern int Image$$ER_IROM1$$RO$$Limit;

struct dflash_device
{
    struct rt_device                rtdev;
    struct rt_mutex                 lock;
    uint32_t                        sector_size;
    uint32_t                        start_addr;
    uint32_t                        disk_size;
};


static rt_err_t rt_dflash_init (rt_device_t dev)
{
    struct dflash_device * dflash_dev;

    FLASH_Init();
    dflash_dev = (struct dflash_device*)dev;
    dflash_dev->sector_size = FLASH_GetSectorSize();
    dflash_dev->start_addr = RT_ALIGN(((uint32_t)&Image$$ER_IROM1$$RO$$Limit + dflash_dev->sector_size), dflash_dev->sector_size);
    dflash_dev->disk_size = FLASH_SIZE - dflash_dev->start_addr;
    return RT_EOK;
}

static rt_err_t rt_dflash_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_dflash_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t rt_dflash_indicate(rt_device_t dev, rt_size_t size)
{
    return RT_EOK;
}

static rt_size_t rt_dflash_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    struct dflash_device * dflash_dev;
    uint8_t *p;
    
    dflash_dev = (struct dflash_device*)dev;
    
    p = (uint8_t*)(dflash_dev->start_addr + (pos * dflash_dev->sector_size));
    rt_memcpy(buffer, p, (size * dflash_dev->sector_size));
	return size;
}

static rt_size_t rt_dflash_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    int i;
    struct dflash_device * dflash_dev;
    uint8_t *p;
    
    dflash_dev = (struct dflash_device*)dev;
    
    p = (uint8_t*)(dflash_dev->start_addr + pos * dflash_dev->sector_size);
    for(i=0;i<size;i++)
    {
        __disable_irq();
        FLASH_EraseSector((uint32_t)p);
        FLASH_WriteSector((uint32_t)p, buffer, dflash_dev->sector_size);
        __enable_irq();
        p += dflash_dev->sector_size;
    }
    return size;
}

static rt_err_t rt_dflash_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct dflash_device * dflash_dev;
    dflash_dev = (struct dflash_device*)dev;
    struct rt_device_blk_geometry geometry;
    rt_memset(&geometry, 0, sizeof(geometry));
	switch (cmd)
	{
        case RT_DEVICE_CTRL_BLK_GETGEOME:
            geometry.block_size = dflash_dev->sector_size;
            geometry.bytes_per_sector = dflash_dev->sector_size;
        
            geometry.sector_count = dflash_dev->disk_size/dflash_dev->sector_size;
            rt_memcpy(args, &geometry, sizeof(struct rt_device_blk_geometry));
		break;
	default: 
		break;
	}
	return RT_EOK;
}

rt_err_t rt_dflash_txcomplete(rt_device_t dev, void *buffer)
{
	return RT_EOK;
}

int rt_hw_dflash_init(const char *name)
{
    
    struct dflash_device *dev;
    
    if(rt_device_find(name))
    {
        return -RT_EIO;
    }
    
    dev = rt_malloc(sizeof(struct dflash_device));
    if(!dev)
    {
        return RT_ENOMEM;
    }
    
	dev->rtdev.type         = RT_Device_Class_Block;
	dev->rtdev.rx_indicate  = rt_dflash_indicate;
	dev->rtdev.init         = rt_dflash_init;
	dev->rtdev.open         = rt_dflash_open;
	dev->rtdev.close		= rt_dflash_close;
	dev->rtdev.read 		= rt_dflash_read;
	dev->rtdev.write        = rt_dflash_write;
	dev->rtdev.control      = rt_dflash_control;
	dev->rtdev.user_data	= RT_NULL;

    /* initialize mutex */
    if (rt_mutex_init(&dev->lock, name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        return -RT_ENOSYS;
    }
    
    rt_device_register(&dev->rtdev, name, RT_DEVICE_FLAG_RDWR);
    return RT_EOK;
}

    
INIT_DEVICE_EXPORT(rt_hw_dflash_init);
