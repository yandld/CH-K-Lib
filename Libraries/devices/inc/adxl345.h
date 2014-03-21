/**
  ******************************************************************************
  * @file    adxl345.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __ADXL345_H__
#define __ADXL345_H__

#include <stdint.h>
#include "i2c_abstraction.h"

typedef struct adxl345_device
{
    // params
    i2c_bus * bus;
    // ops
    i2c_status (*init) (struct adxl345_device * device, uint8_t chip_addr);
    i2c_status (*probe)(struct adxl345_device * device);
    i2c_status (*readXYZ)(struct adxl345_device * device, short *x, short *y, short *z);
    i2c_status (*calibration)(struct adxl345_device * device);
    short (*convert_angle)(short x, short y, short z, short *ax, short *ay, short *az);
    //internal vars
    i2c_device i2c_device;
}adxl345_device;


//!< API functions
i2c_status adxl345_init(struct adxl345_device * device, uint8_t chip_addr);















#endif



