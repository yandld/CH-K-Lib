/**
  ******************************************************************************
  * @file    w25qxx.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "spi_abstraction.h"
#include "w25qxx.h"

#define W25X_PAGE_SIZE          (256)
#define W25X_SECTOR_SIZE        (4096)
#define W25X_PBLOCK_SIZE        (64*1024)

#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


#ifndef USE_USER_MEM
uint8_t SPI_FLASH_BUFFER[4096];	
#endif

typedef struct
{
    const char* name;
    uint32_t size;
    uint16_t id;
}w25qxx_attr_t;

static const w25qxx_attr_t w25qxx_attr_table[] = 
{
    {"W25Q10",    128*1024, 0xEF10},
    {"W25Q20",    256*1024, 0xEF11},
    {"W25Q40",    512*1024, 0xEF12},
    {"W25Q80",   1024*1024, 0xEF13},
    {"W25Q16",   2048*1024, 0xEF14},
    {"W25Q32",   4096*1024, 0xEF15},
    {"W25Q64",   8192*1024, 0xEF16},
    {"W25Q128", 16384*1024, 0xEF17}, 
};

static spi_status w25qxx_power_up(struct w25qxx_device * device)
{
    volatile uint32_t i;
    uint8_t buf[1];
    buf[0] = W25X_ReleasePowerDown;
    device->bus->write(device->bus, &device->spi_device, buf, sizeof(buf), true);
    //delay 3us
    for(i=0;i<1000;i++);
    return kspi_status_ok;
}

static uint8_t w25qxx_read_sr(struct w25qxx_device * device)
{
    uint8_t buf[1];
    buf[0] = W25X_ReadStatusReg;
    device->bus->write(device->bus, &device->spi_device, buf, 1, false); //false = 保持片选,继续发送
    device->bus->read(device->bus, &device->spi_device, buf, 1, true);
    return buf[0];
}


static spi_status w25qxx_probe(struct w25qxx_device * device)
{
    uint32_t i;
    uint16_t id;
    uint8_t buf[4];
    buf[0] = W25X_ManufactDeviceID;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
    //read id
    device->bus->write(device->bus, &device->spi_device, buf, 4, false);
    device->bus->read(device->bus, &device->spi_device, buf, 2, true);
    id = ((buf[0]<<8) + buf[1]);
    //see if we find a match
    for(i = 0; i< ARRAY_SIZE(w25qxx_attr_table);i++)
    {
        if(w25qxx_attr_table[i].id == id)
        {
            // find a match
            device->name = w25qxx_attr_table[i].name;
            device->id = w25qxx_attr_table[i].id;
            device->size = w25qxx_attr_table[i].size;
            w25qxx_power_up(device);
            return kspi_status_ok; 
        }
    }
    return kspi_status_error;
}

static spi_status w25qxx_read(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t buf_send[4];
    buf_send[0] = W25X_ReadData;
    buf_send[1] = (uint8_t)((addr)>>16);
    buf_send[2] = (uint8_t)((addr)>>8);
    buf_send[3] = (uint8_t)addr;
    if(device->bus->write(device->bus, &device->spi_device, buf_send, sizeof(buf_send), false))
    {
        return kspi_status_error;
    }
    device->bus->read(device->bus, &device->spi_device, buf, len, true);
    return kspi_status_ok;
}

static spi_status w25qxx_write_enable(struct w25qxx_device * device)
{
    uint8_t buf[1];
    buf[0] = W25X_WriteEnable;
    device->bus->write(device->bus, &device->spi_device, buf, sizeof(buf), true);
    return kspi_status_ok;
}

static spi_status w25qxx_write_page(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t buf_send[4];
    w25qxx_write_enable(device);
    buf_send[0] = W25X_PageProgram;
    buf_send[1] = (uint8_t)((addr)>>16);
    buf_send[2] = (uint8_t)((addr)>>8);
    buf_send[3] = (uint8_t)addr;
    if(device->bus->write(device->bus, &device->spi_device, buf_send, sizeof(buf_send), false))
    {
        return kspi_status_error;
    }
    device->bus->write(device->bus, &device->spi_device, buf, len, true);
    // wait busy
    while((w25qxx_read_sr(device) & 0x01) == 0x01);
    return kspi_status_ok;
}

spi_status w25qxx_write_no_check(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)  
{ 			 		 
	uint16_t pageremain;	   
    #if DEBUG
    printf("w25qxx - write_no_check: addr:%d len:%d\r\n", addr, len);
    #endif
	pageremain = W25X_PAGE_SIZE-(addr%W25X_PAGE_SIZE); //单页剩余的字节数		 	    
	if(len <= pageremain) pageremain = len;//不大于256个字节
	while(1)
	{	   
        if(w25qxx_write_page(device, addr, buf, pageremain))
        {
            return kspi_status_error;
        }
		if(len == pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			buf += pageremain;
			addr += pageremain;	
			len -= pageremain;			  //减去已经写入了的字节数
			if(len > 256)pageremain=256; //一次可以写入256个字节
			else pageremain = len; 	  //不够256个字节了
		}
	}
    return kspi_status_ok;
} 

static spi_status w25qxx_erase_sector(struct w25qxx_device * device, uint32_t addr)
{
    uint8_t buf_send[4];
    addr /= W25X_SECTOR_SIZE;
    addr *= W25X_SECTOR_SIZE; //round addr to N x W25X_SECTOR_SIZE
    buf_send[0] = W25X_SectorErase;
    buf_send[1] = (uint8_t)((addr)>>16);
    buf_send[2] = (uint8_t)((addr)>>8);
    buf_send[3] = (uint8_t)addr;
    w25qxx_write_enable(device);
    while((w25qxx_read_sr(device) & 0x01) == 0x01);
    device->bus->write(device->bus, &device->spi_device, buf_send, sizeof(buf_send), true);
    while((w25qxx_read_sr(device) & 0x01) == 0x01);
    return kspi_status_ok;
}

spi_status w25qxx_write(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)  
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * mem_pool;
    mem_pool = SPI_FLASH_BUFFER;
 	secpos = addr/4096;//扇区地址  
	secoff = addr%4096;//在扇区内的偏移
	secremain = 4096-secoff;//扇区剩余空间大小   

 	if(len <= secremain)secremain = len;//不大于4096个字节
	while(1) 
	{	
        w25qxx_read(device, secpos*4096, mem_pool, 4096); //读出整个扇区的内容
		for(i = 0; i < secremain; i++)//校验数据
		{
			if(mem_pool[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
            w25qxx_erase_sector(device, secpos*4096); //擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制原来的数据
			{
				mem_pool[i+secoff]=buf[i];	  
			}
            w25qxx_write_no_check(device, secpos*4096, mem_pool,4096); //写入整个扇区 
		}else 
        {
            w25qxx_write_no_check(device, addr, buf, secremain);//写已经擦除了的,直接写入扇区剩余区间. 
        }			   
		if(len == secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	buf += secremain;  //指针偏移
			addr += secremain;//写地址偏移	   
		   	len -= secremain;				//字节数递减
			if(len > 4096) secremain = 4096;	//下一个扇区还是写不完
			else secremain = len;			//下一个扇区可以写完了
		}	 
	}
    return kspi_status_ok;
}






spi_status w25qxx_init(struct w25qxx_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate)
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
    if(device->bus->bus_config(device->bus, device->spi_device.bus_chl, kspi_cpol0_cpha1, baudrate))
    {
        return kspi_status_error;
    }
    // link ops
    device->init = w25qxx_init;
    device->probe = w25qxx_probe;
    device->read = w25qxx_read;
    device->write = w25qxx_write;
    device->erase_sector = w25qxx_erase_sector;
    return kspi_status_ok;
}









