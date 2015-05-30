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

#include "bootloader_common.h"
#include "bootloader/context.h"
#include "device/fsl_device_registers.h"
#include "drivers/uart/scuart.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/* UART PIN Defintion */
#define UART_TX_PORT                PORTD
#define UART_TX_PIN                 7
#define UART_TX_ALT                 3
#define UART_RX_PORT                PORTD
#define UART_RX_GPIO                PTD
#define UART_RX_IRQn                PORTD_IRQn
#define UART_RX_ON_PORTD
#define UART_RX_IRQ_TRIGGER_FLAG    0x0a // Failling Edge
#define UART_RX_PIN                 6
#define UART_RX_ALT                 3

/* I2C PIN Defintion */
#define I2C_SCL_PORT    PORTB
#define I2C_SCL_PIN     2
#define I2C_SCL_ALT     2
#define I2C_SDA_PORT    PORTB
#define I2C_SDA_PIN     3
#define I2C_SDA_ALT     3

/* SPI PIN Defintion */
#define SPI_PCS_PORT    PORTD
#define SPI_PCS_PIN     0
#define SPI_PCS_ALT     2
#define SPI_SCK_PORT    PORTD
#define SPI_SCK_PIN     1
#define SPI_SCK_ALT     2
#define SPI_SOUT_PORT   PORTD
#define SPI_SOUT_PIN    2
#define SPI_SOUT_ALT    2
#define SPI_SIN_PORT    PORTD
#define SPI_SIN_PIN     3
#define SPI_SIN_ALT     2

/* BOOT PIN Definition */
#define BOOTPIN_PORT   PORTE
#define BOOTPIN_GPIO   PTE
#define BOOTPIN_PIN    26 
#define BOOTPIN_DEBOUNCE_READ_COUNT 1000

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////
//! this is to store the function pointer for calling back to the function that wants
//! the UART RX instance pin that triggered the interrupt.
static pin_irq_callback_t s_pin_irq_func = NULL;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/* This function is called for configurating pinmux for uart module
 * This function only support switching default or gpio or fixed-ALTx mode on fixed pins
 * (Although there are many ALTx-pinmux configuration choices on various pins for the same
 * peripheral module) */
void uart_pinmux_config(unsigned int instance, pinmux_type_t pinmux)
{
    switch(pinmux)
    {
    case kPinmuxType_Default:
        PORT_BWR_PCR_MUX(UART_TX_PORT, UART_TX_PIN, 0);
        PORT_BWR_PCR_MUX(UART_RX_PORT, UART_RX_PIN, 0);
        break;
    case kPinmuxType_GPIO:
        PORT_BWR_PCR_MUX(UART_RX_PORT, UART_RX_PIN, UART_RX_ALT);           // Set UART_RX pin in GPIO mode
        GPIO_CLR_PDDR(UART_RX_GPIO, 1 << UART_RX_PIN);                      // Set UART_RX pin as an input
        break;
    case kPinmuxType_Peripheral:
        PORT_BWR_PCR_MUX(UART_TX_PORT, UART_TX_PIN, UART_TX_ALT);   // Set UART_RX pin to UART_RX functionality
        PORT_BWR_PCR_MUX(UART_RX_PORT, UART_RX_PIN, UART_RX_ALT);   // Set UART_TX pin to UART_TX functionality
        break;
    default:
        break;
    }
}

/* This function is called for configurating pinmux for i2c module
 * This function only support switching default or gpio or fixed-ALTx mode on fixed pins
 * (Although there are many ALTx-pinmux configuration choices on various pins for the same
 * peripheral module) 
 */
void i2c_pinmux_config(unsigned int instance, pinmux_type_t pinmux)
{
    switch(pinmux)
    {
    case kPinmuxType_Default:
        PORT_BWR_PCR_MUX(I2C_SCL_PORT, I2C_SCL_PIN, 0);
        PORT_BWR_PCR_MUX(I2C_SCL_PORT, I2C_SCL_PIN, 0);
        PORT_BWR_PCR_ODE(I2C_SDA_PORT, I2C_SDA_PIN, 0);
        PORT_BWR_PCR_ODE(I2C_SDA_PORT, I2C_SDA_PIN, 0);
        break;
    case kPinmuxType_Peripheral:
        // Enable pins for I2C.
        PORT_BWR_PCR_MUX(I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_ALT);
        PORT_BWR_PCR_MUX(I2C_SCL_PORT, I2C_SCL_PIN, I2C_SDA_ALT);
        PORT_BWR_PCR_ODE(I2C_SDA_PORT, I2C_SDA_PIN, 0);
        PORT_BWR_PCR_ODE(I2C_SDA_PORT, I2C_SDA_PIN, 0);
        break;
    }
       
}

/* This function is called for configurating pinmux for spi module
 * This function only support switching default or gpio or fixed-ALTx mode on fixed pins
 * (Although there are many ALTx-pinmux configuration choices on various pins for the same
 * peripheral module) */
void spi_pinmux_config(unsigned int instance, pinmux_type_t pinmux)
{
    switch(pinmux)
    {
    case kPinmuxType_Default:
        PORT_BWR_PCR_MUX(SPI_PCS_PORT, SPI_PCS_PIN, 0);
        PORT_BWR_PCR_MUX(SPI_SCK_PORT, SPI_SCK_PIN, 0);
        PORT_BWR_PCR_MUX(SPI_SOUT_PORT, SPI_SOUT_PIN, 0);
        PORT_BWR_PCR_MUX(SPI_SIN_PORT, SPI_SIN_PIN, 0);
        break;
    case kPinmuxType_Peripheral:
        // Enable pins for SPI
        PORT_BWR_PCR_MUX(SPI_PCS_PORT, SPI_PCS_PIN, SPI_PCS_ALT);
        PORT_BWR_PCR_MUX(SPI_SCK_PORT, SPI_SCK_PIN, SPI_SCK_ALT);
        PORT_BWR_PCR_MUX(SPI_SOUT_PORT, SPI_SOUT_PIN, SPI_SOUT_ALT);
        PORT_BWR_PCR_MUX(SPI_SIN_PORT, SPI_SIN_PIN, SPI_SIN_ALT);
        break;
    default:
        break;
    }
}

void init_hardware(void)
{
    // Disable the MPU otherwise USB cannot access the bus
    MPU->CESR = 0;

    // Enable all the ports
    SIM->SCGC5 |= ( SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );

    // Update SystemCoreClock. FOPT bits set the OUTDIV1 value.
    SystemCoreClock /= (SIM_BRD_CLKDIV1_OUTDIV1(SIM) + 1);
}

void deinit_hardware(void)
{
    SIM->SCGC5 &= (uint32_t)~( SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );

}

bool usb_clock_init(void)
{
    // Select PLL clock
    SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK);

    // Enable USB-OTG IP clocking
    SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);

    // Configure enable USB regulator for device
    SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;

    return true;
}

uint32_t get_bus_clock(void)
{
    uint32_t busClockDivider = SIM_BRD_CLKDIV1_OUTDIV2(SIM) + 1;
    uint32_t coreClockDivider = SIM_BRD_CLKDIV1_OUTDIV1(SIM) + 1;
    return (SystemCoreClock * coreClockDivider / busClockDivider);
}

uint32_t get_uart_clock( unsigned int instance )
{
    switch(instance)
    {
        case 0:
        case 1:
            // UART0 and UART1 always use the system clock
            return SystemCoreClock;
        case 2:
        case 3:
        case 4:
        case 5:
            // UART2, UART3, UART4, and UART5 always use the bus clock
            return get_bus_clock();
        default:
            return 0;
    }
}

unsigned int read_autobaud_pin( unsigned int instance )
{   
    return (GPIO_RD_PDIR(UART_RX_GPIO) >> UART_RX_PIN) & 1;
}

bool is_boot_pin_asserted(void)
{
#ifdef BL_TARGET_FLASH
    // Initialize boot pin for GPIO
    PORT_BWR_PCR_MUX(BOOTPIN_PORT, BOOTPIN_PIN, 1);
    // Set boot pin as an input
    GPIO_CLR_PDDR(BOOTPIN_GPIO, 1 << BOOTPIN_PIN);
    // Set boot pin pullup enabled, pullup select, filter enable
    PORT_SET_PCR(BOOTPIN_PORT, BOOTPIN_PIN, PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_PFE_MASK);

    unsigned int readCount = 0;

    // Sample the pin a number of times
    for (unsigned int i = 0; i < BOOTPIN_DEBOUNCE_READ_COUNT; i++)
    {
        readCount += (GPIO_RD_PDIR(BOOTPIN_GPIO) >> BOOTPIN_PIN) & 1;
    }

    // boot pin is pulled high so we are measuring lows, make sure most of our measurements
    // registered as low
    return (readCount < (BOOTPIN_DEBOUNCE_READ_COUNT/2));
#else
    // Boot pin for Flash only target
    return false;
#endif
}


void enable_autobaud_pin_irq(pin_irq_callback_t func)
{
    NVIC_SetPriority(UART_RX_IRQn, 1);
    NVIC_EnableIRQ(UART_RX_IRQn);
    // Only look for a falling edge for our interrupts
    PORT_BWR_PCR_IRQC(UART_RX_PORT, UART_RX_PIN, UART_RX_IRQ_TRIGGER_FLAG);
    s_pin_irq_func = func;
}

void disable_autobaud_pin_irq(void)
{
    NVIC_SetPriority(UART_RX_IRQn, 0);
    NVIC_DisableIRQ(UART_RX_IRQn);
    // Only look for a falling edge for our interrupts
    PORT_BWR_PCR_IRQC(UART_RX_PORT, UART_RX_PIN, 0);
}

//! @brief this is going to be used for autobaud IRQ handling for UART
#if defined (UART_RX_ON_PORTA)
void PORTA_IRQHandler(void)
{
    // Check if the pin for UART1 is what triggered UART_RX_PORT interrupt
    if (PORT_BRD_PCR_ISF(UART_RX_PORT, UART_RX_PIN) && s_pin_irq_func)
    {
        s_pin_irq_func();
        PORT_WR_ISFR(UART_RX_PORT, ~0U);
    }
}
#elif defined (UART_RX_ON_PORTB)
void PORTB_IRQHandler(void)
{
    // Check if the pin for UART1 is what triggered UART_RX_PORT interrupt
    if (PORT_BRD_PCR_ISF(UART_RX_PORT, UART_RX_PIN) && s_pin_irq_func)
    {
        s_pin_irq_func();
        PORT_WR_ISFR(UART_RX_PORT, ~0U);
    }
}
#elif defined (UART_RX_ON_PORTC)
void PORTC_IRQHandler(void)
{
    // Check if the pin for UART1 is what triggered UART_RX_PORT interrupt
    if (PORT_BRD_PCR_ISF(UART_RX_PORT, UART_RX_PIN) && s_pin_irq_func)
    {
        s_pin_irq_func();
        PORT_WR_ISFR(UART_RX_PORT, ~0U);
    }
}
#elif defined (UART_RX_ON_PORTD)
void PORTD_IRQHandler(void)
{
    // Check if the pin for UART1 is what triggered UART_RX_PORT interrupt
    if (PORT_BRD_PCR_ISF(UART_RX_PORT, UART_RX_PIN) && s_pin_irq_func)
    {
        s_pin_irq_func();
        PORT_WR_ISFR(UART_RX_PORT, ~0U);
    }
}
#elif defined (UART_RX_ON_PORTE)
void PORTD_IRQHandler(void)
{
    // Check if the pin for UART1 is what triggered the PORT C interrupt
    if (PORT_BRD_PCR_ISF(UART_RX_PORT, UART_RX_PIN) && s_pin_irq_func)
    {
        s_pin_irq_func();
        PORT_WR_ISFR(UART_RX_PORT, ~0U);
    }
}
#endif


void update_available_peripherals()
{
}


#if __ICCARM__

size_t __write(int handle, const unsigned char *buf, size_t size)
{
    return size;
}

#endif // __ICCARM__

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

