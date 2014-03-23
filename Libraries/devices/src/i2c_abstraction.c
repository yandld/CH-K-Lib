/**
  ******************************************************************************
  * @file    i2c_abstraction.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "i2c_abstraction.h"
#include "i2c.h"
#include "board.h"

i2c_status i2c_bus_deinit(i2c_bus_t bus)
{
    return ki2c_status_unsupported;
}

i2c_status i2c_bus_read(i2c_bus_t bus, i2c_device_t device, uint8_t *buf, uint32_t len)
{
    int ret = I2C_BurstRead(bus->instance, device->chip_addr, device->subaddr, device->subaddr_len, buf, len);
    if(ret)
    {
        return ki2c_status_error; 
    }
    return ki2c_status_ok;
}

i2c_status i2c_bus_write(i2c_bus_t bus, i2c_device_t device, uint8_t *buf, uint32_t len)
{
    int ret = I2C_BurstWrite(bus->instance, device->chip_addr, device->subaddr, device->subaddr_len, buf, len);
    if(ret)
    {
        return ki2c_status_error; 
    }
    return ki2c_status_ok; 
}

i2c_status i2c_bus_probe(i2c_bus_t bus, uint8_t chip_addr)
{
    i2c_status ret;
    I2C_GenerateSTART(bus->instance);
    I2C_Send7bitAddress(bus->instance, chip_addr, kI2C_Write);
    if(I2C_WaitAck(bus->instance))
    {
      ret = ki2c_status_no_slave;
    }
    else
    {
        ret = ki2c_status_ok;
    }
    I2C_GenerateSTOP(bus->instance);
    while(!I2C_IsBusy(bus->instance));
    return ret;
}

i2c_status i2c_bus_init(struct i2c_bus * bus, uint32_t instance, uint32_t baudrate)
{
    static uint8_t bus_open_flag = 0;
    if(!bus)
    {
        return ki2c_status_error;
    }
    bus->baudrate = baudrate;
    bus->instance = instance;
    if(!bus_open_flag)
    {
        bus->instance = I2C_QuickInit(BOARD_I2C_MAP, 100*1000);
        bus_open_flag = 1;
    }
    bus->init = i2c_bus_init;
    bus->deinit = i2c_bus_deinit;
    bus->read = i2c_bus_read;
    bus->write = i2c_bus_write;
    bus->probe = i2c_bus_probe;
    return ki2c_status_ok;
}

