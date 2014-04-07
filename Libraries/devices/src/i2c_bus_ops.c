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


static int kinetis_i2c_configure(i2c_device_t device, struct i2c_config *configuration)
{
    printf("i2c config\r\n");
    return I2C_EOK;
}

static int kinetis_i2c_read (i2c_device_t device, uint8_t *buf, uint32_t len)
{
    int ret = I2C_BurstRead(device->bus->instance, device->chip_addr, device->subaddr, device->subaddr_len, buf, len);
    if(ret)
    {
        return I2C_ERROR; 
    }
    return I2C_EOK;
}

static int kinetis_i2c_write (i2c_device_t device, uint8_t *buf, uint32_t len)
{
    int ret = I2C_BurstWrite(device->bus->instance, device->chip_addr, device->subaddr, device->subaddr_len, buf, len);
    if(ret)
    {
        return I2C_ERROR; 
    }
    return I2C_EOK;
}

const static struct i2c_ops kinetis_i2c_ops = 
{
    kinetis_i2c_configure,
    kinetis_i2c_read,
    kinetis_i2c_write,
};



int kinetis_i2c_bus_init(struct i2c_bus* bus, uint32_t instance)
{
    uint32_t _instnace;
    /* init hardware */
    _instnace = I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    if(_instnace != instance)
    {
        return 1;
    }
    //PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
   // PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    /* register bus */
    bus->instance = instance;
    return i2c_bus_register(bus, &kinetis_i2c_ops);
}



