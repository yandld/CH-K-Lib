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

#include <rtthread.h>

#include "uart.h"
#include "board.h"

#define UART_BUF_SIZE               (64)

struct uart_device
{
    struct rt_device                rtdev;
    struct rt_mutex                 lock;
    char                            rx_buf[UART_BUF_SIZE];
    uint32_t                        rx_len;
    uint32_t                        hw_instance;
    void (*hw_isr)(uint16_t data);
};

static void _lock(struct uart_device * dev)
{
    rt_mutex_take(&dev->lock, RT_WAITING_FOREVER);
}

static void _unlock(struct uart_device * dev)
{
    rt_mutex_release(&dev->lock);
}

static void UART0_ISR(uint16_t byteReceived)
{
    struct uart_device *dev;

    dev = (struct uart_device *)rt_device_find("uart0");
    if(dev)
    {
        dev->rx_len++;
        dev->rx_buf[(dev->rx_len)-1] = byteReceived;
        if(dev->rtdev.rx_indicate)
        {
            dev->rtdev.rx_indicate((rt_device_t)dev, dev->rx_len); 
        }
    }
}

static void UART1_ISR(uint16_t byteReceived)
{
    struct uart_device *dev;

    dev = (struct uart_device *)rt_device_find("uart1");
    if(dev)
    {
        dev->rx_len++;
        dev->rx_buf[dev->rx_len-1] = byteReceived;
        dev->rtdev.rx_indicate((rt_device_t)dev, dev->rx_len);
    }
}

static void UART2_ISR(uint16_t byteReceived)
{
    struct uart_device *dev;

    dev = (struct uart_device *)rt_device_find("uart2");
    if(dev)
    {
        dev->rx_len++;
        dev->rx_buf[dev->rx_len-1] = byteReceived;
        dev->rtdev.rx_indicate((rt_device_t)dev, dev->rx_len);
    }
}

static void UART3_ISR(uint16_t byteReceived)
{
    struct uart_device *dev;

    dev = (struct uart_device *)rt_device_find("uart3");
    if(dev)
    {
        dev->rx_len++;
        dev->rx_buf[dev->rx_len-1] = byteReceived;
        dev->rtdev.rx_indicate((rt_device_t)dev, dev->rx_len);
    }
}

static void UART4_ISR(uint16_t byteReceived)
{
    struct uart_device *dev;

    dev = (struct uart_device *)rt_device_find("uart4");
    if(dev)
    {
        dev->rx_len++;
        dev->rx_buf[dev->rx_len-1] = byteReceived;
        dev->rtdev.rx_indicate((rt_device_t)dev, dev->rx_len);
    }
}



static rt_err_t rt_uart_init (rt_device_t dev)
{
    struct uart_device * uart_dev;
    uart_dev = (struct uart_device*)dev;
    
    uint32_t instance = UART_QuickInit(BOARD_UART_DEBUG_MAP, BOARD_UART_BAUDRATE);

    UART_EnableTxFIFO(instance, true);
    UART_CallbackRxInstall(HW_UART0, UART0_ISR);
    UART_CallbackRxInstall(HW_UART1, UART1_ISR);
    UART_CallbackRxInstall(HW_UART2, UART2_ISR);
    UART_CallbackRxInstall(HW_UART3, UART3_ISR);
    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(uart_dev->hw_instance, kUART_IT_Rx, true);

    switch(uart_dev->hw_instance)
    {
        case 0:
            uart_dev->hw_isr = UART0_ISR;
            break;
        case 1:
            uart_dev->hw_isr = UART1_ISR;
            break; 
        case 2:
            uart_dev->hw_isr = UART2_ISR;
            break;   
        case 3:
            uart_dev->hw_isr = UART3_ISR;
            break; 
        case 4:
            uart_dev->hw_isr = UART4_ISR;
            break; 
    }
    return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    int len;
    struct uart_device * uart_dev;
    _lock((struct uart_device *)dev);
    uart_dev = (struct uart_device*)dev;
    len = uart_dev->rx_len;
    rt_memcpy(buffer, uart_dev->rx_buf, len);
    uart_dev->rx_len = 0;
    _unlock((struct uart_device *)dev);
    return len;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    volatile uint8_t *p;
    struct uart_device * uart_dev;
    
    _lock((struct uart_device *)dev);
    
    uart_dev = (struct uart_device*)dev;
    p = (uint8_t*)buffer;
    
    while(size--)
    {
        /*
         * to be polite with serial console add a line feed
         * to the carriage return character
         */
        if (*p == '\n' && (dev->open_flag & RT_DEVICE_FLAG_STREAM))
        {
            UART_WriteByte(uart_dev->hw_instance, '\r');
        }
        UART_WriteByte(uart_dev->hw_instance, *p++);
    }
    
    _unlock((struct uart_device *)dev);
    
    return size;
}

static rt_err_t rt_uart_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    return RT_EOK; 
}

int rt_hw_uart_init(const char *name, uint32_t instance)
{
    struct uart_device *dev;
    
    if(rt_device_find(name))
    {
        return -RT_EIO;
    }
    
    dev = rt_malloc(sizeof(struct uart_device));
    if(!dev)
    {
        return RT_ENOMEM;
    }
    
	dev->rtdev.type 		= RT_Device_Class_Char;
	dev->rtdev.rx_indicate  = RT_NULL;
	dev->rtdev.tx_complete  = RT_NULL;
	dev->rtdev.init 		= rt_uart_init;
	dev->rtdev.open         = RT_NULL;
	dev->rtdev.close		= RT_NULL;
	dev->rtdev.read 		= rt_uart_read;
	dev->rtdev.write        = rt_uart_write;
	dev->rtdev.control      = rt_uart_control;
	dev->rtdev.user_data	= RT_NULL;
    dev->hw_instance = instance;
    dev->rx_len = 0;
    
    /* initialize mutex */
    if (rt_mutex_init(&dev->lock, name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        return -RT_ENOSYS;
    }
    
    rt_device_register(&dev->rtdev, name, RT_DEVICE_FLAG_RDWR);
    return RT_EOK;
}


INIT_BOARD_EXPORT(rt_hw_uart_init);
