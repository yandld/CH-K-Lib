#include <stdint.h>
#include "rtt_i2c_bit_ops.h"
#include "i2c_at24cxx.h"

#define AT24CXX_DEBUG

#ifdef AT24CXX_DEBUG
#define AT24CXX_TRACE         rt_kprintf
#else
#define AT24CXX_TRACE(...)
#endif /* #ifdef AT24CXX_DEBUG */

static struct at24cxx_device  at24cxx_device;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

static const struct at24cxx_attr at24cxx_attr_table[] = 
{
    {"at24c01",    128, 8, 0x50},
    {"at24c02",    256, 8, 0x50},
    {"at24c04",    512, 16,0x50},
    {"at24c08",   1024, 16,0x50},
    {"at24c16",   2048, 16,0x50},
};

void at24cxx_test(void)
{
    int i, r;
    rt_uint8_t buf[] ={0x44, 0x43, 0x66};
    struct rt_i2c_bus_device* i2c;
    i2c = rt_i2c_bus_device_find("i2c0");
    struct rt_i2c_msg msg;
    msg.addr = 0x3A>>1;
    msg.buf = buf;
    msg.len = 3;

    r =  rt_i2c_transfer(i2c, &msg, 1);
    //r = rt_i2c_master_send(i2c, 0x3A>>1, RT_I2C_NO_START, (const rt_uint8_t*)&i, 2);
    //r = rt_i2c_master_send(i2c, 0x3A>>1, 0, (const rt_uint8_t*)&i, 1);
    if(r == RT_EOK)
    {
        rt_kprintf("found%X\r\n", r);
        while(1);
    }
}

static rt_err_t control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = at24cxx_device.geometry.bytes_per_sector;
        geometry->sector_count = at24cxx_device.geometry.sector_count;
        geometry->block_size = at24cxx_device.geometry.block_size;
    }

    return RT_EOK;
}

static void lock(struct at24cxx_device * dev)
{
    rt_mutex_take(&dev->lock , RT_WAITING_FOREVER);
}

static void unlock(struct at24cxx_device * dev)
{
    rt_mutex_release(&dev->lock);
}

static int write_page(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    AT24CXX_TRACE("at24c02 write pos=%d size=%d\r\n", pos, size);
    lock(&at24cxx_device);
    pos = pos*at24cxx_device.geometry.bytes_per_sector;
    rt_i2c_master_send(at24cxx_device.rt_i2c_bus_device,
                             at24cxx_device.attr.chip_addr + (pos/256),
                             RT_I2C_WR,
                             buffer,
                             size);
    unlock(&at24cxx_device);
    return size;
}

static rt_size_t read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    int i;
    rt_uint8_t *p = buffer;
    AT24CXX_TRACE("at24c02 read pos=%d size=%d\r\n", pos, size);
    
    lock(&at24cxx_device);
    pos = pos*at24cxx_device.geometry.bytes_per_sector;
    rt_i2c_master_recv(at24cxx_device.rt_i2c_bus_device,
                             at24cxx_device.attr.chip_addr + (pos/256),
                             RT_I2C_RD,
                             buffer,
                             size);

    unlock(&at24cxx_device);
    
 //   for(i = 0; i < size; i++)
 //   {
  //      rt_kprintf("0x%X ", *p++);
  //  }

    return size;
}

/* RT-Thread device interface */
static rt_err_t init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t close(rt_device_t dev)
{
    return RT_EOK;
}


rt_err_t at24cxx_init(const char * device_name, const char * i2c_bus_name)
{
    int i;
    struct rt_i2c_bus_device* i2c_bus_device;
    
    /* initialize mutex */
    if (rt_mutex_init(&at24cxx_device.lock, device_name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("init sd lock mutex failed\n");
        return -RT_ENOSYS;
    }

    /* we got i2c bus */
    i2c_bus_device = rt_i2c_bus_device_find(i2c_bus_name);
    if(i2c_bus_device == RT_NULL)
    {
        AT24CXX_TRACE("i2c device %s not found!\r\n", i2c_bus_device);
        return -RT_ENOSYS;
    }
    at24cxx_device.rt_i2c_bus_device = i2c_bus_device;
    
    /* find match */
    for(i=0;i<ARRAY_SIZE(at24cxx_attr_table);i++)
    {
        if(!rt_strcmp(at24cxx_attr_table[i].name, device_name))
        {
            AT24CXX_TRACE("%s supported hardware:%s\r\n", __func__, device_name);
            at24cxx_device.attr = at24cxx_attr_table[i];
            break;
        }
    }
    
    /* devices not supported */
    if( i == ARRAY_SIZE(at24cxx_attr_table))
    {
        return RT_ERROR;
    }
    
    at24cxx_device.geometry.bytes_per_sector = at24cxx_device.attr.page_size;
    at24cxx_device.geometry.block_size = at24cxx_device.attr.page_size;
    at24cxx_device.geometry.sector_count = at24cxx_device.attr.total_size/at24cxx_device.attr.page_size;
    
    /* register device */
    at24cxx_device.parent.type    = RT_Device_Class_Block;
    at24cxx_device.parent.init    = init;
    at24cxx_device.parent.open    = open;
    at24cxx_device.parent.close   = close;
    at24cxx_device.parent.read 	  = read;
    at24cxx_device.parent.write   = write_page;
    at24cxx_device.parent.control = control;
    /* no private */
    at24cxx_device.parent.user_data = RT_NULL;

    rt_device_register(&at24cxx_device.parent, device_name,
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
    
    return RT_EOK;
}
