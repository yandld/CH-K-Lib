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

#include <stdint.h>

//基本概念
// page :256byte 写入的最小单位 并且必须保证 写入前全部是0xFF
// sectoer: 4KBbyte
// block:   64KBbyte


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

struct w25qxx_init_t
{
    uint32_t (*xfer)(uint8_t *buf_in, uint8_t *buf_out, uint32_t len, uint8_t cs_state);
    uint32_t (*get_reamin)(void);
    void     (*delayms)(uint32_t ms);
};

//!< API functions
int w25qxx_init(struct w25qxx_init_t* init);
int w25qxx_write(uint32_t addr, uint8_t *buf, uint32_t len);
int w25qxx_read(uint32_t addr, uint8_t *buf, uint32_t len);
int w25qxx_write_page(uint32_t addr, uint8_t *buf, uint32_t len);
int w25qxx_erase_sector(uint32_t addr);
int w25qxx_erase_chip(void);
int w25qxx_get_id(void);

#endif 


