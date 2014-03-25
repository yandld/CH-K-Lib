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

struct rt_device uart_device;
static rt_err_t rt_serial_init (rt_device_t dev)
{
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    UART_ITConfig(UART4, UART_IT_RDRF, ENABLE);
	  NVIC_EnableIRQ(UART4_RX_TX_IRQn);
	return RT_EOK;
}

static rt_err_t rt_serial_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_serial_close(rt_device_t dev)
{
	return RT_EOK;
}

 uint8_t gch;
 uint8_t is_read;
static rt_size_t rt_serial_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
		rt_uint8_t* ptr = buffer;
    if(is_read)
		{
			*ptr = gch;
			is_read = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	/*
    rt_uint8_t ch;
		rt_uint8_t* ptr = buffer;
    if(UART_ReceiveData(UART4, &ch) == TRUE)
		{
        *ptr = ch;
			   size = 1;
		}
		else
		{
			size = 0;
		}
	  return size;
	*/
}


static rt_size_t rt_serial_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
		rt_uint8_t* ptr = (rt_uint8_t*)buffer;
    while(size--)
		{
			if(*ptr == '\n')
			{
				UART_SendData(UART4, '\r');
			}
			UART_SendData(UART4, *ptr);
			ptr++;
		}
	
}
void	rt_hw_usart_init(void)
{

	uart_device.type 		= RT_Device_Class_Char;
	uart_device.rx_indicate = RT_NULL;
	uart_device.tx_complete = RT_NULL;
	uart_device.init 		= rt_serial_init;
	uart_device.open		= rt_serial_open;
	uart_device.close		= rt_serial_close;
	uart_device.read 		= rt_serial_read;
	uart_device.write 		= rt_serial_write;
//	uart_device->control 	= rt_serial_control;
	uart_device.user_data	= RT_NULL;
	
	 rt_device_register(&uart_device, "uart", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);
}







