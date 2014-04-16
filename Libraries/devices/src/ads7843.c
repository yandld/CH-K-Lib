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

static struct spi_device device;

uint32_t ads7843_readX(int * value)
{
    uint8_t buf[2];
    buf[0] = ADS7843_CMD_READ_X;
    spi_write(&device, buf, 1, false);
    spi_read(&device, buf, 2, true);
    *value = ((buf[0]<<8) + buf[1])>>4; //12bit mode
    return SPI_EOK;
}

uint32_t ads7843_readY(int * value)
{
    uint8_t buf[2];
    buf[0] = ADS7843_CMD_READ_Y;
    spi_write(&device, buf, 1, false);
    spi_read(&device, buf, 2, true);
    *value = ((buf[0]<<8) + buf[1])>>4; //12bit mode
    return SPI_EOK;
}


int ads7843_init(spi_bus_t bus, int cs)
{
    uint32_t ret;
    device.csn = cs;
    device.config.baudrate = 2*1000*1000;
    device.config.data_width = 8;
    device.config.mode = SPI_MODE_0 | SPI_MASTER | SPI_MSB;
    ret = spi_bus_attach_device(bus, &device);
    if(ret)
    {
        return ret;
    }
    else
    {
        ret = spi_config(&device);
    }
    return ret;
}

