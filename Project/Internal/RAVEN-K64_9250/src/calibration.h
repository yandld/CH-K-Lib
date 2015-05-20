/**
  ******************************************************************************
  * @file    calibration.h
  * @author  YANDLD
  * @version V2.5
  * @date    2015.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef _CALIBERATION_H_
#define _CALIBERATION_H_

#include <stdint.h>



void veep_init(void);
void veep_write(uint8_t *buf, uint32_t len);
void veep_read(uint8_t *buf, uint32_t len);


#endif

