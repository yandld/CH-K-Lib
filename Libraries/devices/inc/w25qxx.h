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



//!< API functions
int w25qxx_init(spi_bus_t bus, uint32_t cs);
int w25qxx_probe(void);
int w25qxx_write(uint32_t addr, uint8_t *buf, uint32_t len);
int w25qxx_read(uint32_t addr, uint8_t *buf, uint32_t len);
int w25qxx_erase_chip(void);
uint32_t w25qxx_get_size(void);
uint32_t w25qxx_get_id(void);
const char * w25qxx_get_name(void);


#endif 


