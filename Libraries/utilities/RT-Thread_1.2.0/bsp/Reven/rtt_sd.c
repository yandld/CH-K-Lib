#include <rthw.h>
#include <rtthread.h>
#include "sd.h"


static struct rt_device sd_device;
static  rt_mutex_t mutex;
    
static rt_err_t rt_sd_init (rt_device_t dev)
{
    int r;
    r = SD_QuickInit(20*1000*1000);
    if(r)
    {
        rt_kprintf("SD hardware init failed code:%d\r\n", r);
        return RT_ERROR;
    }
    mutex = rt_mutex_create("sd_mutex", RT_IPC_FLAG_FIFO);
    return RT_EOK;
}

static rt_err_t rt_sd_open(rt_device_t dev, rt_uint16_t oflag)
{
	rt_kprintf("sd driver - I need open\r\n");
	return RT_EOK;
}

static rt_err_t rt_sd_close(rt_device_t dev)
{
    rt_mutex_delete(mutex);
	rt_kprintf("sd driver - I need close\r\n");
	return RT_EOK;
}

rt_err_t rt_sd_indicate(rt_device_t dev, rt_size_t size)
{
    rt_kprintf("I need indicate\r\n"); 
    return RT_EOK;
}

static rt_size_t rt_sd_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    int r;
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
   // r = SD_ReadSingleBlock(pos, (rt_uint8_t *)buffer);
    r = SD_ReadMultiBlock(pos, (rt_uint8_t *)buffer, size);
    rt_mutex_release(mutex);
    if(r)
    {
        rt_kprintf("sd_read error!%d\r\n", r);
        return 0;
    }
	return size;
}


static rt_size_t rt_sd_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    int r;
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    r = SD_WriteMultiBlock(pos, (rt_uint8_t *)buffer, size);
    rt_mutex_release(mutex);
    if(r)
    {
        rt_kprintf("sd_write error!\r\n", r);
        return 0;
    }
    return size;
}

static rt_err_t rt_sd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    uint32_t size;
    struct rt_device_blk_geometry geometry;
    rt_memset(&geometry, 0, sizeof(geometry));
	switch (cmd)
	{
	case RT_DEVICE_CTRL_BLK_GETGEOME:
		geometry.block_size = 512;
		geometry.bytes_per_sector = 512;
        size = SD_GetSizeInMB();
		geometry.sector_count = size*1024*2;
		rt_memcpy(args, &geometry, sizeof(struct rt_device_blk_geometry));
		break;
	default: 
		break;
	}
	return RT_EOK;

}

rt_err_t rt_sd_txcomplete(rt_device_t dev, void *buffer)
{
	return RT_EOK;
}

void rt_hw_sd_init(uint32_t instance, const char *name)
{

	sd_device.type 		= RT_Device_Class_Block;
	sd_device.rx_indicate = rt_sd_indicate;
	sd_device.tx_complete = RT_NULL;
	sd_device.init 		= rt_sd_init;
	sd_device.open		= rt_sd_open;
	sd_device.close		= rt_sd_close;
	sd_device.read 		= rt_sd_read;
	sd_device.write     = rt_sd_write;
	sd_device.control 	= rt_sd_control;
	sd_device.user_data	= rt_sd_txcomplete;
	
    rt_device_register(&sd_device, name, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE | RT_DEVICE_FLAG_REMOVABLE);
}


