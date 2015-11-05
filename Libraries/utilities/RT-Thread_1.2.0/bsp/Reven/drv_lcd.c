#include <rthw.h>
#include <rtthread.h>
#include "ili9320.h"
#include "rtt_drv.h"
#include "finsh.h"

struct lcd_device
{
    struct rt_device                rtdev;
    struct rt_mutex                 lock;
    uint32_t                        id;
};


static rt_err_t rt_lcd_init (rt_device_t dev)
{
    return ili9320_init();
}


static rt_err_t rt_lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
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
	dev->rtdev.init         = rt_lcd_init;
	dev->rtdev.open         = RT_NULL;
	dev->rtdev.close		= RT_NULL;
	dev->rtdev.read 		= RT_NULL;
	dev->rtdev.write        = RT_NULL;
	dev->rtdev.control      = rt_lcd_control;
	dev->rtdev.user_data	= RT_NULL;

    /* initialize mutex */
    rt_mutex_init(&dev->lock, name, RT_IPC_FLAG_FIFO);
    rt_device_register(&dev->rtdev, name, RT_DEVICE_FLAG_RDWR);
    return RT_EOK;
}

int  lcd_info(int argc, char** argv)
{
    uint32_t id;
    rt_device_t lcd = rt_device_find("lcd0");
    
    rt_lcd_init(RT_NULL);
    rt_lcd_control(RT_NULL, RTGRAPHIC_CTRL_GET_INFO, &id);
    rt_kprintf("lcd controllor:0x%X\r\n", id);
    rt_kprintf("lcd size:%dx%d\r\n", LCD_X_MAX, LCD_Y_MAX);
    return RT_EOK;
}

MSH_CMD_EXPORT(lcd_info, lcd_info);

