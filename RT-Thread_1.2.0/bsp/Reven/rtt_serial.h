#ifndef __RT_HW_SERIAL_H__
#define __RT_HW_SERIAL_H__

#include "uart.h"
#include <rthw.h>
#include <rtthread.h>

typedef struct kinetis_uart_device * uart_device_t;
#define UART_RX_BUFFER_SIZE     (32)
struct kinetis_uart_device
{
    rt_uint32_t instance;
    rt_uint8_t  rx_buffer[UART_RX_BUFFER_SIZE];
	rt_uint32_t rx_rec_index;
    rt_uint32_t rx_read_index;
};


#endif