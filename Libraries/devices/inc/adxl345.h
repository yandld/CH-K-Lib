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

typedef struct adxl345_device *adxl345_device_t;
struct adxl345_device
{
    // params
    struct i2c_bus * bus;
    // ops
    i2c_status (*init) (struct adxl345_device * device, uint8_t chip_addr);
    i2c_status (*probe)(adxl345_device_t device);
    i2c_status (*readXYZ)(adxl345_device_t device, short *x, short *y, short *z);
    i2c_status (*calibration)(adxl345_device_t device);
    short (*convert_angle)(short x, short y, short z, short *ax, short *ay, short *az);
    //internal vars
    struct i2c_device i2c_device;
};


//!< API functions
i2c_status adxl345_init(struct adxl345_device * device, uint8_t chip_addr);















#endif



