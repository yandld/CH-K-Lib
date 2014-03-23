/**
  ******************************************************************************
  * @file    at24cxx.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "i2c_abstraction.h"
#include "at24cxx.h"
#include "string.h"
/**
  ******************************************************************************
  * @supported chip    AT24C01
  *                    AT24C02
  *                    AT24C04
  *                    AT24C08
  *                    AT24C16
  ******************************************************************************
  */
static const uint32_t AT24CXX_TotalSizeTable[] = {128, 256, 512, 1024, 2048};
static const uint32_t AT24CXX_PageSizeTable[] =  {8, 8, 16, 16, 16};

#define AT24CXX_CHIP_ADDRESS   (0x50)

static i2c_status at24cxx_probe(ad24cxx_device_t device, at24cxx_part_number_t type)
{
    if(!device->bus)
    {
        return ki2c_status_error;
    }
    if(!device->bus->init)
    {
        return ki2c_status_error;
    }
    if(!device->bus->probe)
    {
        return ki2c_status_error; 
    }
    if(device->bus->probe(device->bus, AT24CXX_CHIP_ADDRESS))
    {
        return ki2c_status_error;
    }
    device->type = type;
    return ki2c_status_ok;
}

static i2c_status at24cxx_read(ad24cxx_device_t device,uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
    if(!device->bus)
    {
        return ki2c_status_error;
    }
    if(!device->bus->read)
    {
        return ki2c_status_error;
    }
    device->i2c_device.chip_addr = (addr/256) + AT24CXX_CHIP_ADDRESS;
    device->i2c_device.subaddr = addr%256;
    device->i2c_device.subaddr_len = 1;
    ret = device->bus->read(device->bus, &device->i2c_device, buf, len);
    return (i2c_status)ret;
}

static i2c_status at24cxx_write_page(ad24cxx_device_t device, uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
    if(!device->bus)
    {
        return ki2c_status_error;
    }
    if(!device->bus->write)
    {
        return ki2c_status_error;
    }
    device->i2c_device.chip_addr = (addr/256) + AT24CXX_CHIP_ADDRESS;
    device->i2c_device.subaddr = addr%256;
    device->i2c_device.subaddr_len = 1;
    ret = device->bus->write(device->bus, &device->i2c_device, buf, len);
    return (i2c_status)ret;
}

static i2c_status at24cxx_write(ad24cxx_device_t device, uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
	uint32_t secpos;
	uint32_t secoff;
	uint16_t Byteremian;
	uint32_t pageremain;
	secpos = addr/AT24CXX_PageSizeTable[device->type];
	secoff = addr%AT24CXX_PageSizeTable[device->type];
	pageremain = AT24CXX_PageSizeTable[device->type]-secoff;
    // check vailidiation
    if((addr + len ) > AT24CXX_TotalSizeTable[device->type])
    {
        return ki2c_status_unsupported;
    }
    ret = at24cxx_write_page(device, addr, buf, len);
    if(ret)
    {
        return (i2c_status)ret;
    }
    while(device->bus->probe(device->bus, AT24CXX_CHIP_ADDRESS) != ki2c_status_ok);
	addr += pageremain;
	secpos++;
	Byteremian = len - pageremain;
	while(Byteremian != 0)
	{
		if(Byteremian <= AT24CXX_PageSizeTable[device->type])
		{
			 pageremain = Byteremian;
		}
		else
		{
			pageremain = AT24CXX_PageSizeTable[device->type];
		}
        ret = at24cxx_write_page(device, secpos*AT24CXX_PageSizeTable[device->type], buf, pageremain);
        if(ret)
        {
            return (i2c_status)ret;
        }
        while(device->bus->probe(device->bus, AT24CXX_CHIP_ADDRESS) != ki2c_status_ok);
        secpos++;
        buf += pageremain;
        Byteremian -= pageremain;
	}
    return ki2c_status_ok;
}

static i2c_status self_test(ad24cxx_device_t device)
{
    int ret;
    uint8_t buf[8],buf1[8],buf2[8];
    ret = device->read(device, 0, buf, sizeof(buf));
    if(ret)
    {
        return (i2c_status)ret;
    }
    memset(buf1,'T',sizeof(buf1));
    ret = device->write(device, 0, buf1, sizeof(buf1));
    if(ret)
    {
        return (i2c_status)ret;
    }
    ret = device->read(device, 0, buf2, sizeof(buf2));
    if(ret)
    {
        return (i2c_status)ret;
    }
    ret = device->write(device, 0, buf, sizeof(buf));
    if(ret)
    {
        return (i2c_status)ret;
    }
    if(memcmp(buf1, buf2, sizeof(buf1)))
    {
        return ki2c_status_error;
    }
    return ki2c_status_ok;
}

static i2c_status at24cxx_get_size(ad24cxx_device_t device, uint32_t * size)
{
    *size = AT24CXX_TotalSizeTable[device->type];
    return ki2c_status_ok;
}

i2c_status at24cxx_init(struct at24cxx_device * device)
{
    if(!device->bus)
    {
        return ki2c_status_error;
    }
    if(!device->bus->init)
    {
        return ki2c_status_error;
    }
    if(device->bus->init(device->bus, device->bus->instance, device->bus->baudrate))
    {
        return ki2c_status_error;
    }
    // link ops
    device->init = at24cxx_init;
    device->probe = at24cxx_probe;
    device->read = at24cxx_read;
    device->write = at24cxx_write;
    device->self_test = self_test;
    device->get_size = at24cxx_get_size;
    return ki2c_status_ok;
}


