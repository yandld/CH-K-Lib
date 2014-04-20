/**
  ******************************************************************************
  * @file    nrf24l01.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __NRF24L01_H
#define __NRF24L01_H	 		  

#include <stdint.h>
#include "spi_abstraction.h"
#include "gpio.h"


#ifdef RAVEN
#define NRF24L01_CE_HIGH()     do {PEout(0) = 1;} while(0)
#define NRF24L01_CE_LOW()      do {PEout(0) = 0;} while(0)
#elif URANUS
#define NRF24L01_CE_HIGH()     do {PCout(2) = 1;} while(0)
#define NRF24L01_CE_LOW()      do {PCout(2) = 0;} while(0)
#else
#warning  "no NRF24L01 CE operation defined!"
#define NRF24L01_CE_HIGH()     void(0)
#define NRF24L01_CE_LOW()      void(0)
#endif

//<! API functions
void nrf24l01_set_tx_mode(void);
void nrf24l01_set_rx_mode(void);
int nrf24l01_write_packet(uint8_t *buf, uint32_t len);
int nrf24l01_read_packet(uint8_t *buf, uint32_t *len);
int nrf24l01_probe(void);
int nrf24l01_init(spi_bus_t bus, uint32_t cs);


#endif











