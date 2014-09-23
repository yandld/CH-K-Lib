#ifndef I2C_AT24CXX_H_INCLUDED
#define I2C_AT24CXX_H_INCLUDED

#include <rtthread.h>
#include <drivers/i2c.h>


struct at24cxx_device
{
    struct rt_device                eep_device;
    struct rt_device_blk_geometry   geometry;
    struct rt_i2c_bus_device *      rt_i2c_bus_device;
    struct rt_mutex                 lock;
};

extern rt_err_t at24cxx_init(const char * device_name,
                            const char * i2c_bus_name);

#endif
