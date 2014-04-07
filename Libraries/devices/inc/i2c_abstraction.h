/**
  ******************************************************************************
  * @file    i2c_abstraction.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __I2C_ABSTRACTION_H__
#define __I2C_ABSTRACTION_H__

#include <stdint.h>
#include "i2c_abstraction.h"

typedef enum
{
    ki2c_status_ok = 0,
    ki2c_status_error,
    ki2c_status_unsupported,
    ki2c_status_busy,
    ki2c_status_no_slave,
}i2c_status;

typedef struct i2c_device *i2c_device_t;
struct i2c_device
{
    uint8_t     chip_addr;        /*!< The slave's 7-bit address.*/
    uint32_t    subaddr;
    uint32_t    subaddr_len;
};


typedef struct i2c_bus* i2c_bus_t;
struct i2c_bus
{
    // params
    uint32_t instance;
    uint32_t baudrate;
    // ops
    i2c_status (*deinit)(i2c_bus_t bus);
    i2c_status (*read)(i2c_bus_t bus, i2c_device_t device, uint8_t *buf, uint32_t len);
    i2c_status (*write)(i2c_bus_t bus, i2c_device_t device, uint8_t *buf, uint32_t len);
    i2c_status (*probe)(i2c_bus_t bus, uint8_t chip_addr);
};


//!< API funtctions
i2c_status i2c_bus_init(struct i2c_bus * bus, uint32_t instance, uint32_t baudrate);
i2c_status i2c_bus_probe(i2c_bus_t bus, uint8_t chip_addr);
i2c_status i2c_bus_write(i2c_bus_t bus, i2c_device_t device, uint8_t *buf, uint32_t len);
i2c_status i2c_bus_read(i2c_bus_t bus, i2c_device_t device, uint8_t *buf, uint32_t len);

#endif


