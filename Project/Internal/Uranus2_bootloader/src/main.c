#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "dma.h"
#include "pit.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "flash.h"

#define BOOTLOADER_VERSION      (100)

int main(void)
{
    DelayInit();

    
    UART_Init(UART0_RX_PA01_TX_PA02, 115200);
    UART_SetIntMode(HW_UART0, kUART_IntRx, true);
    printf("HelloWorld!\r\n");
 
    
    while(1)
    {
  
    }
}



void UART0_IRQHandler(void)
{
    uint8_t ch;
    if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART_C2_RIE_MASK))
    {
        ch = UART0->D;
    }
    /**
        this is very important because in real applications
        OR usually occers, the RDRF will block when OR is asserted 
    */
    if(UART0->S1 & UART_S1_OR_MASK)
    {
        ch = UART0->D;
    }
}

void PIT_IRQHandler(void)
{
    if(PIT->CHANNEL[0].TFLG)
    {
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;

    }
}

void PORTA_IRQHandler(void)
{
   // PORTA->ISFR |= (1<<MPU9250_INT_PIN);

}
