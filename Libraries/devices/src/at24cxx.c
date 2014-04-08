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

struct at24cxx_attr
{
    const char* name;
    uint32_t total_size; /* total size */
    uint16_t page_size;  /* page size */
    uint8_t  chip_addr;  /* base addr */
};

static const struct at24cxx_attr at24cxx_attr_table[] = 
{
    {"at24c01",    128, 8, 0x50},
    {"at24c02",    256, 8, 0x50},
    {"at24c04",    512, 16,0x50},
    {"at24c08",   1024, 16,0x50},
    {"at24c16",   2048, 16,0x50},
};



static struct i2c_device device;
static struct at24cxx_attr at24cxx_addr;

int at24cxx_get_size(void)
{
    return at24cxx_addr.total_size;
}

int at24cxx_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
    device.chip_addr = (addr/256) + at24cxx_addr.chip_addr;
    device.subaddr = addr%256;
    device.subaddr_len = 1;
    ret = device.bus->ops->read(&device, buf, len);
    return ret;
}

int at24cxx_write_page(uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
    device.chip_addr = (addr/256) + at24cxx_addr.chip_addr;
    device.subaddr = addr%256;
    device.subaddr_len = 1;
    ret = device.bus->ops->write(&device, buf, len);
    return ret;
}

int at24cxx_write(uint32_t addr, uint8_t *buf, uint32_t len)
{
    int ret;
	uint32_t secpos;
	uint32_t secoff;
	uint16_t Byteremian;
	uint32_t pageremain;
    uint32_t page_size = at24cxx_addr.page_size;
	secpos = addr/page_size;
	secoff = addr%page_size;
	pageremain = page_size - secoff;
    /* check vailidiation */
    if((addr + len ) > at24cxx_addr.total_size)
    {
        return 1;
    }
    ret = at24cxx_write_page(addr, buf, len);
    if(ret)
    {
        return ret;
    }
    while(i2c_probe(&device) != I2C_EOK);
	addr += pageremain;
	secpos++;
	Byteremian = len - pageremain;
	while(Byteremian != 0)
	{
		if(Byteremian <= page_size)
		{
			 pageremain = Byteremian;
		}
		else
		{
			pageremain = page_size;
		}
        ret = at24cxx_write_page(secpos*page_size, buf, pageremain);
        if(ret)
        {
            return ret;
        }
        while(i2c_probe(&device) != I2C_EOK);
        secpos++;
        buf += pageremain;
        Byteremian -= pageremain;
	}
    return 0;
}


int at24cxx_self_test(void)
{
    int ret;
    uint8_t buf[128],buf1[128],buf2[128];
    ret = at24cxx_read(0, buf, sizeof(buf));
    if(ret)
    {
        return ret;
    }
    memset(buf1,'Y',sizeof(buf1));
    ret = at24cxx_write(0, buf1, sizeof(buf1));
    if(ret)
    {
        return ret;
    }
    ret = at24cxx_read(0, buf2, sizeof(buf2));
    if(ret)
    {
        return ret;
    }
    ret = at24cxx_write(0, buf, sizeof(buf));
    if(ret)
    {
        return ret;
    }
    if(!memcmp(buf1, buf2, sizeof(buf1)))
    {
        return 0;
    }
    return 1;
}

int at24cxx_init(struct i2c_bus* bus, const char * name)
{
    uint32_t i;
    uint32_t ret;
    /* find match */
    for(i=0;i<ARRAY_SIZE(at24cxx_attr_table);i++)
    {
        if(!strcmp(at24cxx_attr_table[i].name, name))
        {
            at24cxx_addr = at24cxx_attr_table[i];
            break;
        }
    }
    if( i == ARRAY_SIZE(at24cxx_attr_table))
    {
        return 2;
    }
    
    device.config.baudrate = 100*1000;
    device.config.data_width = 8;
    device.config.mode = 0;
    device.subaddr_len = 1;
    device.chip_addr = at24cxx_addr.chip_addr;
    ret = i2c_bus_attach_device(bus, &device);
    if(ret)
    {
        return ret;
    }
    else
    {
        ret = i2c_config(&device);
    }
    return ret;
}






