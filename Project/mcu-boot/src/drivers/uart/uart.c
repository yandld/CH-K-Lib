/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_platform_common.h"
#include "uart.h"
#include <stdarg.h>

static uart_data_sink_func_t s_uart_data_sink_callback;

/********************************************************************/
/*
 * Initialize the uart for 8N1 operation, interrupts disabled, and
 * no hardware flow-control
 *
 * NOTE: Since the uarts are pinned out in multiple locations on most
 *       Kinetis devices, this driver does not enable uart pin functions.
 *       The desired pins should be enabled before calling this init function.
 *
 * Parameters:
 *  uartch      uart channel to initialize
 *  uartclk     uart module Clock in Hz(used to calculate baud)
 *  baud        uart baud rate
 */
status_t uart_init (UART_Type * uartch, int uartclk, int baud, uart_data_sink_func_t function)
{
    uint8_t temp;
    register uint16_t sbr = (uint16_t)(uartclk/(baud * 16));
    int baud_check = uartclk / (sbr * 16);
    uint32_t baud_diff;

    s_uart_data_sink_callback = function;

    if (baud_check > baud)
    {
        baud_diff = baud_check - baud;
    }
    else
    {
        baud_diff = baud - baud_check;
    }

    // If the baud rate cannot be within 3% of the passed in value
    // return a failure
    if (baud_diff > ((baud / 100) * 3))
    {
        return kStatus_Fail;
    }

    if (uartch == UART1)
    {
        SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
    }
    else
    {
        SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    }
    //Make sure that the transmitter and receiver are disabled while we
    //change settings.

    uartch->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );

    // Configure the uart for 8-bit mode, no parity
    uartch->C1 = 0;    // We need all default settings, so entire register is cleared

    // Save off the current value of the uartx_BDH except for the SBR field
    temp = uartch->BDH & ~(UART_BDH_SBR(0x1F));

    uartch->BDH = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
    uartch->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);

    // Enable receive interrupts
    uartch->C2 |= BM_UART_C2_RIE;

    switch((uint32_t)uartch)
    {
        case (uint32_t)UART0:
            NVIC_EnableIRQ(UART0_IRQn);
            break;
        case (uint32_t)UART1:
            NVIC_EnableIRQ(UART1_IRQn);
            break;
        case (uint32_t)UART2:
            NVIC_EnableIRQ(UART2_IRQn);
            break;
    }

    // Enable receiver and transmitter
    uartch->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );

    return kStatus_Success;
}

/********************************************************************/
/*
 * UART1 IRQ Handler
 *
 */
void UART1_IRQHandler(void)
{
    if (UART1->S1 & UART_S1_RDRF_MASK)
    {
        s_uart_data_sink_callback(UART1->D);
    }
}

/********************************************************************/
/*
 * UART2 IRQ Handler
 *
 */
void UART2_IRQHandler(void)
{
    if (UART2->S1 & UART_S1_RDRF_MASK)
    {
        s_uart_data_sink_callback(UART2->D);
    }
}

/********************************************************************/
/*
 * Wait for space in the uart Tx FIFO and then send a character
 *
 * Parameters:
 *  channel      uart channel to send to
 *  ch             character to send
 */
void uart_putchar (UART_Type * channel, char ch)
{
    // Wait until space is available in the FIFO
    while(!(channel->S1 & UART_S1_TDRE_MASK));

    // Send the character
    channel->D = (uint8_t)ch;
}

/********************************************************************/
/*
 * Shutdown UART1 or UART2
 *
 * Parameters:
 *  uartch      the UART to shutdown
 *
 */
void uart_shutdown (UART_Type * uartch)
{
    // Disable interrupt
    switch((uint32_t)uartch)
    {
        case (uint32_t)UART0:
            NVIC_DisableIRQ(UART0_IRQn);
            break;
        case (uint32_t)UART1:
            NVIC_DisableIRQ(UART1_IRQn);
            break;
        case (uint32_t)UART2:
            NVIC_DisableIRQ(UART2_IRQn);
            break;
    }

    // In case uart peripheral isn't active which also means uart clock doesn't open,
    // So enable clocking to UARTx in any case, then we can write control register.
    if (uartch == UART1)
    {
        SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
    }
    else
    {
        SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    }

    // Reset all control registers.
    uartch->C2 = 0;
    //uartch->C1 = 0;
    //uartch->C3 = 0;
    //uartch->C4 = 0;

    // Gate the uart clock.
    if (uartch == UART1)
    {
        HW_SIM_SCGC4_CLR(SIM_BASE, BM_SIM_SCGC4_UART1);
    }
    else
    { // Assume UART2
        HW_SIM_SCGC4_CLR(SIM_BASE, BM_SIM_SCGC4_UART2);
    }
}

