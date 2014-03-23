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


typedef struct ads7843_device * ads7843_device_t;
struct ads7843_device
{
    // params
    spi_bus_t bus;
    // ops
    spi_status (*init) (struct ads7843_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate);
    spi_status (*probe)(ads7843_device_t device);
    spi_status (*readX)(ads7843_device_t device, uint16_t * value);
    spi_status (*readY)(ads7843_device_t device, uint16_t * value);
    //internal vars
    struct spi_device spi_device;
};


//!< API functions
spi_status ads7843_init(struct ads7843_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate);





#endif
  
  


