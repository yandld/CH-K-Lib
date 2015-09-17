#include <rthw.h>
#include <rtthread.h>
#include "ili9320.h"
#include "rtt_drv.h"

struct lcd_device
{
    struct rt_device                rtdev;
    struct rt_mutex                 lock;
    uint32_t                        id;
};


static rt_err_t _init (rt_device_t dev)
{
    return ili9320_init();
}


static rt_err_t _control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    
	switch (cmd)
	{
        case RTGRAPHIC_CTRL_GET_INFO:
            *(uint32_t*)args = ili9320_get_id();
            break;
        default: 
            break;
	}
	return RT_EOK;
}


int rt_hw_lcd_init(const char *name)
{
    
    struct lcd_device *dev;
    
    if(rt_device_find(name))
    {
        return -RT_EIO;
    }
    
    dev = rt_malloc(sizeof(struct lcd_device));
    if(!dev)
    {
        return RT_ENOMEM;
    }
    
	dev->rtdev.type         = RT_Device_Class_Graphic;
	dev->rtdev.rx_indicate  = RT_NULL;
	dev->rtdev.init         = _init;
	dev->rtdev.open         = RT_NULL;
	dev->rtdev.close		= RT_NULL;
	dev->rtdev.read 		= RT_NULL;
	dev->rtdev.write        = RT_NULL;
	dev->rtdev.control      = _control;
	dev->rtdev.user_data	= RT_NULL;

    /* initialize mutex */
    rt_mutex_init(&dev->lock, name, RT_IPC_FLAG_FIFO);
    rt_device_register(&dev->rtdev, name, RT_DEVICE_FLAG_RDWR);
    return RT_EOK;
}

    
