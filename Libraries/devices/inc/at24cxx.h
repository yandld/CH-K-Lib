/**
  ******************************************************************************
  * @file    at24cxx.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __24CXX_H__
#define __24CXX_H__

#include <stdint.h>
#include "i2c_abstraction.h"

typedef enum
{
    kAT24C01,
    kAT24C02,
    kAT24C04,
    kAT24C08,
    kAT24C16,
}at24cxx_part_number_t;

typedef struct at24cxx_device
{
    // params
    i2c_bus * bus;
    at24cxx_part_number_t type;
    // ops
    i2c_status (*init) (struct at24cxx_device * device);
    i2c_status (*probe)(struct at24cxx_device * device, at24cxx_part_number_t type);
    i2c_status (*read)(struct at24cxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len);
    i2c_status (*write)(struct at24cxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len);
    i2c_status (*self_test)(struct at24cxx_device * device);
    i2c_status (*get_size)(struct at24cxx_device * device, uint32_t * size);
    //internal vars
    i2c_device i2c_device;
}at24cxx_device;

//API funtctions
i2c_status at24cxx_init(struct at24cxx_device * device);


#endif

