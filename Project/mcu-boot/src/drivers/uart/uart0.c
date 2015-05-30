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
#include "uart0.h"
#include <stdarg.h>

static uart0_data_sink_func_t s_uart0_data_sink_callback;

enum
{
    //! The lower the OSR value the better chance we have at hitting a larger multitude
    //! of baud rates in the 0 - 115200 range, minimum for LPUART is 4
    kUART0_OSR_Value = 4U
};

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
 *  uart0clk    uart module Clock in Hz(used to calculate baud)
 *  baud        uart baud rate
 */
status_t uart0_init (uint32_t uart0clk, uint32_t baud, uart0_data_sink_func_t function)
{
    // Calculate SBR rounded to nearest whole number, 14.4 = 14, 14.6 = 15
    // This gives the least error possible
    uint32_t sbr = (uint32_t)(((uart0clk * 10)/(baud * kUART0_OSR_Value)) + 5) / 10;
    uint32_t calculatedBaud = (uint32_t)(uart0clk/(sbr * kUART0_OSR_Value));
    uint32_t baudDiff = MAX(calculatedBaud, baud) - MIN(calculatedBaud, baud);

    if (baudDiff < ((baud / 100) * 3))
    {
        s_uart0_data_sink_callback = function;

        // Enable clocking to UART0
        SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;

        // Disable UART0 before changing registers
        UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

        // Enable both edge since we are using a OSR value between 3 and 8
        UART0->C5 |= UART0_C5_BOTHEDGE_MASK;

        // Setup OSR value
        uint32_t temp = UART0->C4;
        temp &= ~UART0_C4_OSR_MASK;
        temp |= UART0_C4_OSR(kUART0_OSR_Value - 1); // BaudRate = UARTClk / ((OSR + 1) * SBR)

        // Write temp to C4 register
        UART0->C4 = temp;

        // Save off the current value of the uartx_BDH except for the SBR field
        temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));

        UART0->BDH = temp |  UART0_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART0->BDL = (uint8_t)(sbr & UART0_BDL_SBR_MASK);

        // Enable receive interrupts
        UART0->C2 |= BM_UART0_C2_RIE;

        NVIC_EnableIRQ(UART0_IRQn);

        // Enable receiver and transmitter
        UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK );

        return kStatus_Success;
    }
    else
    {
        // Unacceptable baud rate difference
        // More than 3% difference!!
        return kStatus_Fail;
    }

}

/********************************************************************/
/*
 * UART0 IRQ Handler
 *
 */
void UART0_IRQHandler(void)
{
    s_uart0_data_sink_callback(UART0->D);
}

/********************************************************************/
/*
 * Wait for space in the uart Tx FIFO and then send a character
 *
 * Parameters:
 *  channel      uart channel to send to
 *  ch             character to send
 */
void uart0_putchar (char ch)
{
      // Wait until space is available in the FIFO
      while(!(UART0->S1 & UART0_S1_TDRE_MASK));

      // Send the character
      UART0->D = (uint8_t)ch;
}

/********************************************************************/

/********************************************************************/
/*
 * Shuts down UART0
 *
 * Parameters:
 */
void uart0_shutdown (void)
{
    // Disable interrupt
    NVIC_DisableIRQ(UART0_IRQn);

    // In case uart peripheral isn't active which also means uart clock doesn't open,
    // So enable clocking to UART0 in any case, then we can write control register.
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;

    // Reset all control registers.
    UART0->C2 = 0;
    //UART0->C1 = 0;
    //UART0->C3 = 0;
    UART0->C4 = 0xf;
    UART0->C5 = 0;

    // Gate the uart clock.
    HW_SIM_SCGC4_CLR(SIM_BASE, BM_SIM_SCGC4_UART0);
}
/********************************************************************/

