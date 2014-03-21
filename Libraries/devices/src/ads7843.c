/**
  ******************************************************************************
  * @file    ads7843.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "spi_abstraction.h"
#include "ads7843.h"


#define ADS7843_CMD_READ_X   (0xD0)
#define ADS7843_CMD_READ_Y   (0X90)

static spi_status ads7843_probe(struct ads7843_device * device)
{

    return kspi_status_unsupported;
}

static spi_status ads7843_readX(struct ads7843_device * device, uint16_t * value)
{
    uint8_t buf[2];
    buf[0] = ADS7843_CMD_READ_X; 
    device->bus->write(device->bus, &device->spi_device, buf, 1, false);
    device->bus->read(device->bus, &device->spi_device, buf, 2, true);
    *value = ((buf[0]<<8) + buf[1])>>4; //12bit mode
    return kspi_status_ok;
}

static spi_status ads7843_readY(struct ads7843_device * device, uint16_t * value)
{
    uint8_t buf[2];
    buf[0] = ADS7843_CMD_READ_Y; 
    device->bus->write(device->bus, &device->spi_device, buf, 1, false);
    device->bus->read(device->bus, &device->spi_device, buf, 2, true);
    *value = ((buf[0]<<8) + buf[1])>>4;//12bit mode
    return kspi_status_ok;
}


spi_status ads7843_init(struct ads7843_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate)
{
    if(!device)
    {
        return kspi_status_error;
    }
    device->spi_device.csn = csn;
    device->spi_device.bus_chl = bus_chl;
    device->spi_device.cs_state = kspi_cs_keep_asserted;
    if(!device->bus)
    {
        return kspi_status_error;
    }
    if(!device->bus->init)
    {
        return kspi_status_error;
    }
    if(!device->bus->bus_config)
    {
        return kspi_status_error;
    }
    //init 
    if(device->bus->init(device->bus, device->bus->instance))
    {
        return kspi_status_error;
    }
    // bus chl config
    if(device->bus->bus_config(device->bus, device->spi_device.bus_chl, kspi_cpol0_cpha0, baudrate))
    {
        return kspi_status_error;
    }
    // link ops
    device->init = ads7843_init;
    device->probe = ads7843_probe;
    device->readX = ads7843_readX;
    device->readY = ads7843_readY;
    return kspi_status_ok;
}



