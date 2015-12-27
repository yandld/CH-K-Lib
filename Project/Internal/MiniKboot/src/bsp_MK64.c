#include "bl_cfg.h"
#include "bl_bsp.h"
#include "bl_core.h"

#define BL_UART     UART0

static uint32_t uart_clock_get(void)
{
    return SystemCoreClock;
}

#define INVALID_VAL 0xFFFFFFFFUL

static uint32_t s_clkDivider;

void bl_hw_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK \
               | SIM_SCGC5_PORTB_MASK  \
               | SIM_SCGC5_PORTC_MASK  \
               | SIM_SCGC5_PORTD_MASK \
               | SIM_SCGC5_PORTE_MASK;
    // Select FLL as UART clock source
    SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
    
    s_clkDivider = SIM->CLKDIV1;
    // Set Clock divider
    SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV3(0)|SIM_CLKDIV1_OUTDIV4(1);
    
    PORTD->PCR[6] = PORT_PCR_MUX(3); // UART_RX
    PORTD->PCR[7] = PORT_PCR_MUX(3); // UART_TX
}

void bl_hw_deinit(void)
{
    SIM->SCGC5 &= ~(SIM_SCGC5_PORTA_MASK \
               | SIM_SCGC5_PORTB_MASK  \
               | SIM_SCGC5_PORTC_MASK  \
               | SIM_SCGC5_PORTD_MASK \
               | SIM_SCGC5_PORTE_MASK);
    SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    SIM->CLKDIV1 = s_clkDivider;
}


void bl_hw_interface_init(void)
{ 
    uint32_t uartclk = uart_clock_get();
    uint32_t baud = BL_UART_BAUDRATE;
    uint8_t i;
    uint32_t calculatedBaud = 0;
    uint32_t baudDiff = 0;
    uint32_t brfaVal = 0;
    uint32_t sbrVal;
    uint32_t temp = 0;

    // Calculate the SBR with a BRFA of 0
    sbrVal = uartclk / (16 * baud);
    calculatedBaud = uartclk / (16 * sbrVal);

    // Make sure the SBR is at least a step higher than the baud we want
    // then tune it down by adding the fractional divisor below
    if (calculatedBaud < baud)
    {
        sbrVal -= 1;
    }

    if (calculatedBaud != baud)
    {
        // Now tune to the best BRFA value
        for (i = 1; i <= 32; i++)
        {
            calculatedBaud = (2 * uartclk) / ((32 * sbrVal) + i);

            if (calculatedBaud > baud)
            {
                temp = calculatedBaud - baud;
            }
            else
            {
                temp = baud - calculatedBaud;
            }

            if (temp <= baudDiff)
            {
                baudDiff = temp;
                brfaVal = i;
            }
        }
    }

    if (baudDiff < ((baud / 100) * 3))
    {
        NVIC_EnableIRQ(UART0_RX_TX_IRQn);
        SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;

        //Make sure that the transmitter and receiver are disabled while we
        //change settings.
        BL_UART->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );

        // Configure the uart for 8-bit mode, no parity
        BL_UART->C1 = 0;    // We need all default settings, so entire register is cleared

        // Save off the current value of the uartx_BDH except for the SBR field
        temp = BL_UART->BDH & ~(UART_BDH_SBR(0x1F));

        BL_UART->BDH = temp |  UART_BDH_SBR(((sbrVal & 0x1F00) >> 8));
        BL_UART->BDL = (uint8_t)(sbrVal & UART_BDL_SBR_MASK);
        BL_UART->C4 = (BL_UART->C4 & ~(UART_C4_BRFA_MASK)) | (brfaVal << UART_C4_BRFA_SHIFT);

        // Flush the RX and TX FIFO's
        BL_UART->CFIFO = UART_CFIFO_RXFLUSH_MASK | UART_CFIFO_TXFLUSH_MASK;

        // Enable receive interrupts
        BL_UART->C2 |= UART_C2_RIE_MASK;

        // Enable receiver and transmitter
        BL_UART->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
    }
}

void bl_hw_if_write(const uint8_t *buffer, uint32_t length)
{
    while(length--)
    {
        while(!(BL_UART->S1 & UART_S1_TDRE_MASK))
        {
        }
        BL_UART->D = *buffer++;
    }
}


bool stay_in_bootloader(void)
{
    uint32_t *vectorTable = (uint32_t*)APPLICATION_BASE;
    if (vectorTable[0] == INVALID_VAL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UART0_RX_TX_IRQHandler(void)
{
    if (UART0->S1 & UART_S1_RDRF_MASK)
    {
        bootloader_data_sink(UART0->D);
    }
}

