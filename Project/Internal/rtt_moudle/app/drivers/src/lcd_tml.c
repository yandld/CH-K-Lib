#include <rthw.h>
#include <rtthread.h>
#include <api.h>

static struct rt_device lcdt_device;

static rt_err_t rt_lcdt_init (rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_lcdt_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}


static rt_size_t rt_lcdt_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	return size;
}

static rt_size_t rt_lcdt_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    GUI_DispString(buffer);
    return size;
}

static rt_err_t rt_lcdt_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	return RT_EOK;

}

rt_err_t rt_lcdt_txcomplete(rt_device_t dev, void *buffer)
{
	return RT_EOK;
}

void rt_hw_lcdt_init(void)
{

	lcdt_device.type 		= RT_Device_Class_Char;
	lcdt_device.rx_indicate = RT_NULL;
	lcdt_device.tx_complete = RT_NULL;
	lcdt_device.init 		= rt_lcdt_init;
	lcdt_device.open		= rt_lcdt_open;
	lcdt_device.close		= RT_NULL;
	lcdt_device.read 		= rt_lcdt_read;
	lcdt_device.write       = rt_lcdt_write;
	lcdt_device.control 	= rt_lcdt_control;
	lcdt_device.user_data	= RT_NULL;

    rt_device_register(&lcdt_device, "lcdt0", RT_DEVICE_FLAG_RDWR);
}


