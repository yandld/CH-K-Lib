#include <rthw.h>
#include <rtthread.h>
#include "chlib_k.h"

static struct rt_device sd_device;
static  rt_mutex_t mutex;

#define IFLASH_OFFSET       (0x70000)
#define BLOCK_SIZE          (0x800)

static rt_err_t rt_sd_init (rt_device_t dev)
{
    uint32_t clock;
    uint32_t flash_clock = CLOCK_GetClockFrequency(kFlashClock, &clock);
    
    /* func:Init is SSD API */    
    Init(0, clock, 2); 
    return RT_EOK;
}

static rt_err_t rt_sd_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_sd_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t rt_sd_indicate(rt_device_t dev, rt_size_t size)
{
    return RT_EOK;
}

static rt_size_t rt_sd_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    int i,j;
    char *p = buffer;
    for(j=0;j<BLOCK_SIZE;j++)
    {
        *p++ = *(char*)(IFLASH_OFFSET + pos*BLOCK_SIZE + j);
    }
	return size;
}

static rt_size_t rt_sd_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    int r;
    const char *p = buffer;
    DisableInterrupts();
    EraseSector(IFLASH_OFFSET+pos*BLOCK_SIZE);
    ProgramPage(IFLASH_OFFSET+pos*BLOCK_SIZE+512*0, 512, (char*)p);p+=512;
    ProgramPage(IFLASH_OFFSET+pos*BLOCK_SIZE+512*1, 512, (char*)p);p+=512;
    ProgramPage(IFLASH_OFFSET+pos*BLOCK_SIZE+512*2, 512, (char*)p);p+=512;
    ProgramPage(IFLASH_OFFSET+pos*BLOCK_SIZE+512*3, 512, (char*)p);
    EnableInterrupts();
    return 1;
}

static rt_err_t rt_iflash_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    uint32_t size;
    struct rt_device_blk_geometry geometry;
    rt_memset(&geometry, 0, sizeof(geometry));
	switch (cmd)
	{
	case RT_DEVICE_CTRL_BLK_GETGEOME:
		geometry.block_size = BLOCK_SIZE;
		geometry.bytes_per_sector = BLOCK_SIZE;
		geometry.sector_count = 70;
		rt_memcpy(args, &geometry, sizeof(struct rt_device_blk_geometry));
		break;
	default: 
		break;
	}
	return RT_EOK;

}

static rt_err_t rt_sd_txcomplete(rt_device_t dev, void *buffer)
{
	return RT_EOK;
}

void rt_hw_iflash_init(uint32_t instance, const char *name)
{

	sd_device.type 		= RT_Device_Class_Block;
	sd_device.rx_indicate = rt_sd_indicate;
	sd_device.tx_complete = RT_NULL;
	sd_device.init 		= rt_sd_init;
	sd_device.open		= rt_sd_open;
	sd_device.close		= rt_sd_close;
	sd_device.read 		= rt_sd_read;
	sd_device.write     = rt_sd_write;
	sd_device.control 	= rt_iflash_control;
	sd_device.user_data	= RT_NULL;
	
    rt_device_register(&sd_device, name, RT_DEVICE_FLAG_RDWR  | RT_DEVICE_FLAG_REMOVABLE|RT_DEVICE_FLAG_STANDALONE);
}

void rt_hw_iflash(void)
{
    rt_hw_iflash_init(0, "iflash");
}
    
INIT_DEVICE_EXPORT(rt_hw_iflash);
