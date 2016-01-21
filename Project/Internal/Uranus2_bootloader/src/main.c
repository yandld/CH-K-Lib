#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "gpio.h"
#include "uart.h"

#include "bl_core.h"
#include "bl_cfg.h"

#define BOOTLOAD_TIMEOUT        (300)
static bool jump = false;

void bl_deinit_interface(void)
{
    UART0->BDH = 0;
    UART0->BDL = 0;
    UART0->C4 = 0;
    UART0->C2 = 0;
    SIM->SCGC4 &= ~SIM_SCGC4_UART0_MASK;
}


uint8_t bl_hw_if_read_byte(void)
{
    uint8_t ch;
    while(UART_GetChar(HW_UART0, &ch) == 1)
    {
        if(jump == true)
        {
            application_run();
        }
    }
    return (ch & 0xFF);
}

void bl_hw_if_write(const uint8_t *buffer, uint32_t length)
{
    while(length--)
    {
        putchar(*buffer++);
    }
}


int main(void)
{
    DelayInit();
    DelayMs(10);
    SysTick_SetTime(100*1000);
    SysTick_SetIntMode(true);
    UART_Init(UART0_RX_PA01_TX_PA02, 115200);
    while(1)
    {
        bootloader_run();
    }
}


void SysTick_Handler(void)
{
    static int timeout;
    if(timeout > BOOTLOAD_TIMEOUT/100)
    {
        SysTick_SetIntMode(false);
        if((bootloader_isActive() == false) && (IsAppAddrValidate() == true))
        {
            jump = true;
        } 
    }
    timeout++;
}

