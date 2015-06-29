#include <stdint.h>
#include <rtthread.h>
#include <drivers/spi.h>

static struct rt_device  ads7843_device;
struct rt_spi_device * rt_spi_device;

#define ADS7843_CMD_READ_X   (0xD0)
#define ADS7843_CMD_READ_Y   (0X90)

static rt_size_t _read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_uint8_t send_buffer[1];
    send_buffer[0] = ADS7843_CMD_READ_X; 
    rt_spi_send_then_recv(rt_spi_device, send_buffer, 1, buffer, 2);
    send_buffer[0] = ADS7843_CMD_READ_Y; 
    rt_spi_send_then_recv(rt_spi_device, send_buffer, 1, (void*)((rt_uint32_t)buffer + 2), 2);
    
    return size;
}

static rt_err_t _init (rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t _open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK; 
}

rt_err_t ads7843_init(const char * name, const char * spi_device_name)
{

    rt_spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
    if(rt_spi_device == RT_NULL)
    {
        rt_kprintf("spi device %s not found!\r\n", spi_device_name);
        return -RT_ENOSYS;
    }
    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz = 2 * 1000 * 1000;
        rt_spi_configure(rt_spi_device, &cfg);
    }

    /* register device */
    ads7843_device.type    = RT_Device_Class_Block;
    ads7843_device.init    = _init;
    ads7843_device.open    = _open;
    ads7843_device.close   = RT_NULL;
    ads7843_device.read    = _read;
    ads7843_device.write   = RT_NULL;
    ads7843_device.control = RT_NULL;
    /* no private */
    ads7843_device.user_data = RT_NULL;

    rt_device_register(&ads7843_device, name,
                       RT_DEVICE_FLAG_RDWR);

    return RT_EOK;
}
