/*
 * File      : serial.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-13     bernard      first version
 * 2012-05-15     lgnq         modified according bernard's implementation.
 * 2012-05-28     bernard      code cleanup
 * 2012-11-23     bernard      fix compiler warning.
 * 2013-02-20     bernard      use RT_SERIAL_RB_BUFSZ to define
 *                             the size of ring buffer.
 */

#include <rthw.h>
#include <rtthread.h>
#include "uart.h"

#include "rtt_serial.h"

static struct rt_device uart_device;
static struct kinetis_uart_device serial;
void rt_uart_rx_isr (rt_device_t dev, uint8_t ch)
{
    uart_device_t uart_device = (uart_device_t)(dev->user_data);
    /* save character */
    uart_device->rx_buffer[uart_device->rx_rec_index++] = ch;
    if(uart_device->rx_rec_index >= UART_RX_BUFFER_SIZE)
        uart_device->rx_rec_index = 0;
    
    /* if the next position is read index, discard this 'read char' */
    if (uart_device->rx_rec_index == uart_device->rx_read_index)
    {
        uart_device->rx_read_index++;
        if(uart_device->rx_read_index >= UART_RX_BUFFER_SIZE)
            uart_device->rx_read_index = 0;
    }
    dev->rx_indicate(dev, 1);
}

void UART_RX_ISR(uint8_t ch)
{
    rt_interrupt_enter();
    rt_uart_rx_isr(&uart_device, ch);
    rt_interrupt_leave();
}

static rt_err_t rt_serial_init (rt_device_t dev)
{ 
    if(!(dev->flag & RT_DEVICE_FLAG_ACTIVATED))
    {
        uart_device_t uart_device = (uart_device_t)(dev->user_data);
        uart_device->instance = UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
        rt_memset(uart_device->rx_buffer, 0, sizeof(uart_device->rx_buffer));
        uart_device->rx_rec_index = 0;
        uart_device->rx_read_index = 0;
    }
    dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
    return RT_EOK;
}

static rt_err_t rt_serial_open(rt_device_t dev, rt_uint16_t oflag)
{
    uart_device_t uart_device = (uart_device_t)(dev->user_data);
    if(dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        UART_CallbackRxInstall(uart_device->instance, UART_RX_ISR);
        UART_ITDMAConfig(uart_device->instance, kUART_IT_Rx); 
    }
	return RT_EOK;
}

static rt_err_t rt_serial_close(rt_device_t dev)
{
    uart_device_t uart_device = (uart_device_t)(dev->user_data);
    if(dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        UART_ITDMAConfig(uart_device->instance, kUART_IT_Rx_Disable); 
    }
	return RT_EOK;
}

static rt_size_t rt_serial_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    uint8_t *ptr = buffer;
    uart_device_t uart_device = (uart_device_t)(dev->user_data);
    while(size)
    {
        if (uart_device->rx_read_index != uart_device->rx_rec_index)
        {
            /* read a character */
            *ptr++ = uart_device->rx_buffer[uart_device->rx_read_index];
            size--;

            /* move to next position */
            uart_device->rx_read_index ++;
            if (uart_device->rx_read_index >= UART_RX_BUFFER_SIZE)
                uart_device->rx_read_index = 0;
        }
        else
        {
            break;
        }
    }
    return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}


static rt_size_t rt_serial_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    uart_device_t uart_device = (uart_device_t)(dev->user_data);
    rt_uint8_t* ptr = (rt_uint8_t*)buffer;
    while(size--)
    {
        if(*ptr == '\n')
        {
            UART_WriteByte(uart_device->instance, '\r');
        }
        UART_WriteByte(uart_device->instance, *ptr);
        ptr++;
    }
    return size;
}

rt_err_t rt_hw_serial_register(rt_device_t device, const char* name, rt_uint32_t flag, struct kinetis_uart_device *serial)
{
	device->type 		= RT_Device_Class_Char;
	device->rx_indicate = RT_NULL;
	device->tx_complete = RT_NULL;
	device->init 		= rt_serial_init;
	device->open		= rt_serial_open;
	device->close		= rt_serial_close;
	device->read 		= rt_serial_read;
	device->write 		= rt_serial_write;
//	uart_device->control 	= rt_serial_control;
	device->user_data	= serial; 
    rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM|RT_DEVICE_FLAG_INT_RX);
}


void rt_hw_usart_init(void)
{
    rt_hw_serial_register(&uart_device, "uart", 0, &serial); 
}






