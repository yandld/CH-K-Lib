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



typedef struct ads7843_device
{
    // params
    spi_bus * bus;
    // ops
    spi_status (*init) (struct ads7843_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate);
    spi_status (*probe)(struct ads7843_device * device);
    spi_status (*readX)(struct ads7843_device * device, uint16_t * value);
    spi_status (*readY)(struct ads7843_device * device, uint16_t * value);
    //internal vars
    spi_device spi_device;
}ads7843_device;


//!< API functions
spi_status ads7843_init(struct ads7843_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate);





#endif
  
  


