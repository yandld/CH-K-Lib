#include <stdint.h>
#include "rtt_i2c_bit_ops.h"
#include <drivers/i2c.h>

void at24cxx__test(void)
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

