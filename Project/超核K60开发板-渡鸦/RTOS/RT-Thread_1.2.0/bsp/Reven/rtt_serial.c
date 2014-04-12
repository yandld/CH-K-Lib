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

#include "rtt_serial.h"
#include <rtdevice.h>

static struct rt_serial_device serial;
static struct serial_ringbuffer uart_int_rx;
static char grev_ch = 0;
static bool grev_flag = false;
static void UART_ISR(uint8_t byteReceived)
{
    /* enter interrupt */
    rt_interrupt_enter();
    grev_ch = byteReceived;
    grev_flag = true;
    rt_hw_serial_isr(&serial);
    /* leave interrupt */
    rt_interrupt_leave();
}

static rt_err_t kinetis_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    UART_InitTypeDef UART_InitStruct1;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    UART_InitStruct1.instance = HW_UART0;
    switch(cfg->baud_rate)
    {
        case BAUD_RATE_9600:
            UART_InitStruct1.baudrate = 9600;
            break;
        case BAUD_RATE_115200:
            UART_InitStruct1.baudrate = 115200;
            break;
    }
    
    UART_Init(&UART_InitStruct1);
    
    UART_CallbackRxInstall(HW_UART0, UART_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx);
	return RT_EOK;
}

static rt_err_t kinetis_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        UART_ITDMAConfig(HW_UART0, kUART_IT_Rx_Disable);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        UART_ITDMAConfig(HW_UART0, kUART_IT_Rx);
        break;
    }

    return RT_EOK;
}

static int kinetis_putc(struct rt_serial_device *serial, char c)
{
    UART_WriteByte(HW_UART0, c);
    return 1;
}

static int kinetis_getc(struct rt_serial_device *serial)
{
    int c;
    c = -1;
    if(grev_flag)
    {
        c = grev_ch;
        grev_flag = false;
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


int rt_hw_usart_init(void)
{
    struct serial_configure config;
    config.baud_rate = BAUD_RATE_115200;
    config.bit_order = BIT_ORDER_LSB;
    config.data_bits = DATA_BITS_8;
    config.parity    = PARITY_NONE;
    config.stop_bits = STOP_BITS_1;
    config.invert    = NRZ_NORMAL;
    
    serial.ops    = &kinetis_uart_ops;
    serial.int_rx = &uart_int_rx;
    serial.config = config;
    
    rt_hw_serial_register(&serial, "uart",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
                          RT_NULL);
}



