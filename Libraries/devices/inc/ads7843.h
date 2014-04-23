/**
  ******************************************************************************
  * @file    ads7843.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
  
#ifndef __ADS7843_H__
#define __ADS7843_H__

#include <stdint.h>
#include "spi_abstraction.h"

//!< API functions
int ads7843_init(spi_bus_t bus, int cs);
uint32_t ads7843_readX(int * value);
uint32_t ads7843_readY(int * value);



#endif
  
  


