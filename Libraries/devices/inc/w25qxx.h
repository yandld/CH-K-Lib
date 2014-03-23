/**
  ******************************************************************************
  * @file    w25qxx.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __W25QXX_H__
#define __W25QXX_H__

#include "spi_abstraction.h"

//基本概念
// page :256byte 写入的最小单位 并且必须保证 写入前全部是0xFF
// sectoer: 4KBbyte
// block:   64KBbyte


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

typedef struct w25qxx_device *w25qxx_device_t;

typedef struct w25qxx_device
{
    // params
    spi_bus_t bus;
    // attr
    const char * name;
    uint16_t id;
    uint32_t size;
    // ops
    spi_status (*init) (struct w25qxx_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate);
    spi_status (*probe)(w25qxx_device_t device);
    spi_status (*read)(w25qxx_device_t device, uint32_t addr, uint8_t *buf, uint32_t len);
    spi_status (*write)(w25qxx_device_t device, uint32_t addr, uint8_t *buf, uint32_t len);
    spi_status (*erase_sector)(w25qxx_device_t device, uint32_t addr);
    spi_status (*erase_chip)(w25qxx_device_t device);
    //internal vars
    spi_device spi_device;
}w25qxx_device;


//!< API functions
spi_status w25qxx_init(struct w25qxx_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate);


#endif 


