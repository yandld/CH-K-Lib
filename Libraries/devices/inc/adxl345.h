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



#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif



//!< API functions 
int adxl345_init(struct i2c_bus* bus);
int adxl345_write_register(uint8_t addr, uint8_t value);
int adxl345_probe(void);
int adxl345_get_addr(void);
int adxl345_readXYZ(short *x, short *y, short *z);
int adxl345_calibration(void);
short adxl345_convert_angle(short x, short y, short z, short *ax, short *ay, short *az);




#endif



