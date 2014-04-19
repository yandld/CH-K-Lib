/**
  ******************************************************************************
  * @file    24l01.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __24L01_H
#define __24L01_H	 		  

#include <stdint.h>
#include "spi_abstraction.h"






//<! API functions
uint8_t NRF2401_Init(void);
uint8_t NRF2401_SendPacket(uint8_t *txbuf, uint8_t len);
void NRF2401_SetTXMode(void);
void NRF2401_SetRXMode(void);
uint8_t NRF2401_SendPacket(uint8_t *txbuf, uint8_t len);
int nrf24l01_read_packet(uint8_t *buf, uint32_t *len);
int nrf24l01_probe(void);
int nrf24l01_init(spi_bus_t bus, uint32_t cs);
#endif











