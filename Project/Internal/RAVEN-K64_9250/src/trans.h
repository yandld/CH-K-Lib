#ifndef _TRANS_H_
#define _TRANS_H_

#include <stdint.h>



int uart_dma_init(uint8_t dmaChl, uint32_t uartInstance);
int uart_dma_send(uint8_t* buf, uint32_t size);

#endif
