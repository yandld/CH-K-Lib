/**
  ******************************************************************************
  * @file    spi_abstraction.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __SPI_ABSTRACTION_H__
#define __SPI_ABSTRACTION_H__

#include <stdint.h>
#include <stdbool.h>


typedef enum
{
    kspi_status_ok = 0,
    kspi_status_error,
    kspi_status_unsupported,
    kspi_status_busy,
}spi_status;

typedef enum
{
    kspi_cpol0_cpha0,
    kspi_cpol0_cpha1,
    kspi_cpol1_cpha0,
    kspi_cpol1_cpha1,
}spi_frame_type;

typedef enum
{
    kspi_cs_return_inactive,     
    kspi_cs_keep_asserted,       
}spi_cs_control_type;

typedef struct spi_device *spi_device_t;
struct spi_device
{
    uint32_t                    csn;           //使用SPI的哪根片选信号
    uint32_t                    bus_chl;       //使用哪个传输属性在总线上传输 HW_CTAR0 或者 HW_CTAR1
    spi_cs_control_type         cs_state;      //没传送完一帧数据后是否拉起CS
};

typedef struct spi_bus *spi_bus_t;
struct spi_bus
{
    // params
    uint32_t instance;
    uint32_t baudrate;
    spi_frame_type frame_type;
    // ops
    spi_status (*init)(struct spi_bus * bus, uint32_t instance);
    spi_status (*bus_config)(spi_bus_t bus, uint32_t bus_chl, spi_frame_type frame_type, uint32_t baudrate);
    spi_status (*deinit)(spi_bus_t bus);
    spi_status (*read)(spi_bus_t bus, spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive);
    spi_status (*write)(spi_bus_t bus, spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive);
};


//!< API functinos
spi_status spi_bus_init(struct spi_bus * bus, uint32_t instance);
spi_status spi_bus_read(spi_bus_t bus, spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive);
spi_status spi_bus_write(spi_bus_t bus, spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive);
spi_status bus_config(spi_bus_t bus, uint32_t bus_chl, spi_frame_type frame_type, uint32_t baudrate);


#endif

