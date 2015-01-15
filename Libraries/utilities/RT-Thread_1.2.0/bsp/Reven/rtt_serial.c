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
#include "gpio.h"
#include "board.h"
#include "rtt_serial.h"
#include <rtdevice.h>

static struct rt_serial_device serial;
static struct serial_ringbuffer uart_int_rx;

static uint16_t *gRevCh = RT_NULL;
static void UART_ISR(uint16_t byteReceived)
{
    static uint16_t ch;
    rt_interrupt_enter();
    ch = byteReceived;
    gRevCh = &ch;
    rt_hw_serial_isr(&serial);
    rt_interrupt_leave();
}

static rt_err_t kinetis_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    switch(cfg->baud_rate)
    {
        case BAUD_RATE_9600:
            break;
        case BAUD_RATE_115200:
            break;
    }
	return RT_EOK;
}

static rt_err_t kinetis_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    RT_ASSERT(serial != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        UART_ITDMAConfig(serial->config.reserved, kUART_IT_Rx, false);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        UART_CallbackRxInstall(serial->config.reserved, UART_ISR);
        UART_ITDMAConfig(serial->config.reserved, kUART_IT_Rx, true);
        break;
    }
    return RT_EOK;
}

static int kinetis_putc(struct rt_serial_device *serial, char c)
{
    UART_WriteByte(serial->config.reserved, c);
    return 1;
}

static int kinetis_getc(struct rt_serial_device *serial)
{
    int c;
    c = -1;
    
    if(gRevCh != RT_NULL)
    {
        c = *gRevCh;
        gRevCh = RT_NULL;
    }
    return c;
}

static const struct rt_uart_ops kinetis_uart_ops =
{
    kinetis_configure,
    kinetis_control,
    kinetis_putc,
    kinetis_getc,
};

int rt_hw_usart_init(uint32_t MAP, const char * name)
{
    uint32_t instance;
    instance = UART_QuickInit(BOARD_UART_DEBUG_MAP, BOARD_UART_BAUDRATE);
    UART_EnableTxFIFO(instance, true);
    UART_SetTxFIFOWatermark(instance, UART_GetTxFIFOSize(instance));
    
    struct serial_configure config;
    config.baud_rate = BAUD_RATE_115200;
    config.bit_order = BIT_ORDER_LSB;
    config.data_bits = DATA_BITS_8;
    config.parity    = PARITY_NONE;
    config.stop_bits = STOP_BITS_1;
    config.invert    = NRZ_NORMAL;
    config.reserved  = instance;
    serial.config = config;
    serial.ops    = &kinetis_uart_ops;
    serial.int_rx = &uart_int_rx;
    UART_CallbackRxInstall(instance, UART_ISR);
    UART_ITDMAConfig(instance, kUART_IT_Rx, true);
    return rt_hw_serial_register(&serial, name,
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
                          RT_NULL);

}

static int init_board_uart(void)
{
    rt_hw_usart_init(BOARD_UART_DEBUG_MAP, "uart0");

    return 0;
}

INIT_BOARD_EXPORT(init_board_uart);

