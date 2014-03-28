#include <rthw.h>
#include <rtthread.h>
#include "sd.h"


struct rt_device sd_device;
static  SD_InitTypeDef SD_InitStruct1;
static rt_err_t rt_sd_init (rt_device_t dev)
{
    rt_uint8_t i;

    SD_InitStruct1.SD_BaudRate = 2000000;
    for(i = 0; i< 10; i++)
    {
        if(SD_Init(&SD_InitStruct1) == ESDHC_OK)
        {
            break;
        }
    }
    if(i == 10)
    {
        return RT_ERROR;
    }
    rt_kprintf("Size:%dMB\r\n", SD_InitStruct1.SD_Size);
    return RT_EOK;
}

static rt_err_t rt_sd_open(rt_device_t dev, rt_uint16_t oflag)
{
	rt_kprintf("I need open\r\n");
	return RT_EOK;
}

static rt_err_t rt_sd_close(rt_device_t dev)
{
	rt_kprintf("I need close\r\n");
	return RT_EOK;
}

rt_err_t rt_sd_indicate(rt_device_t dev, rt_size_t size)
{
    rt_kprintf("I need indicate\r\n"); 
}


static rt_size_t rt_sd_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    uint16_t i;
//	DisableInterrupts();
	SD_ReadSingleBlock(pos, buffer);
//	EnableInterrupts();
	//for(i=0;i<512;i++)
//	{
	//	rt_kprintf("%x ", p[i]);
//	}
	return 1;
}


static rt_size_t rt_sd_write (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	  rt_kprintf("pos:%d size:%d\r\n", pos, size);
    SD_WriteSingleBlock(pos, buffer);
		return 1;
}

static rt_err_t rt_sd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    rt_kprintf("I need control:0x%x\r\n", cmd);
    struct rt_device_blk_geometry geometry;
    rt_memset(&geometry, 0, sizeof(geometry));
	switch (cmd)
	{
	case RT_DEVICE_CTRL_BLK_GETGEOME:
		geometry.block_size = 512;
		geometry.bytes_per_sector = 512;
		geometry.sector_count = SD_InitStruct1.SD_Size*1024*2;
		rt_memcpy(args, &geometry, sizeof(struct rt_device_blk_geometry));
		break;
	default:
		break;
	}
	return RT_EOK;

}

rt_err_t rt_sd_txcomplete(rt_device_t dev, void *buffer)
{
	
}

void rt_hw_sd_init(void)
{

	sd_device.type 		= RT_Device_Class_Block;
	sd_device.rx_indicate = rt_sd_indicate;
	sd_device.tx_complete = RT_NULL;
	sd_device.init 		= rt_sd_init;
	sd_device.open		= rt_sd_open;
	sd_device.close		= rt_sd_close;
	sd_device.read 		= rt_sd_read;
	sd_device.write 		= rt_sd_write;
	sd_device.control 	= rt_sd_control;
	sd_device.user_data	= rt_sd_txcomplete;
	
	 rt_device_register(&sd_device, "sd0", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE | RT_DEVICE_FLAG_REMOVABLE);
}