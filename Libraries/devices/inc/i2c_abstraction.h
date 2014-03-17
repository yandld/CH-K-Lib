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

typedef struct
{
    uint8_t     chip_addr;        /*!< The slave's 7-bit address.*/
    uint32_t    subaddr;
    uint32_t    subaddr_len;
}i2c_device;

typedef struct i2c_bus
{
    const char* name;
    i2c_status (*init)(struct i2c_bus * bus, uint32_t instance, uint32_t baudrate);
    i2c_status (*deinit)(struct i2c_bus * bus);
    i2c_status (*read)(struct i2c_bus * bus, i2c_device * device, uint8_t *buf, uint32_t len);
    i2c_status (*write)(struct i2c_bus * bus, i2c_device * device, uint8_t *buf, uint32_t len);
    i2c_status (*probe)(struct i2c_bus * bus, uint8_t chip_addr);
    uint32_t instance;
    uint32_t baudrate;
}i2c_bus;


//!< API funtctions
i2c_status i2c_bus_init(struct i2c_bus * bus, uint32_t instance, uint32_t baudrate);
i2c_status i2c_bus_probe(struct i2c_bus * bus, uint8_t chip_addr);
i2c_status i2c_bus_write(struct i2c_bus * bus, i2c_device * device, uint8_t *buf, uint32_t len);
i2c_status i2c_bus_read(struct i2c_bus * bus, i2c_device * device, uint8_t *buf, uint32_t len);


#endif


