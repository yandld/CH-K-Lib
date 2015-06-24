#include "gpio.h"
#include "common.h"
#include "uart.h"

 
int main(void)
{
    //DelayInit();
    PCC0_PCC_PORTA |= PCC_CLKCFG_CGC_MASK;
    PCC0_PCC_PORTB |= PCC_CLKCFG_CGC_MASK;
    PCC0_PCC_PORTC |= PCC_CLKCFG_CGC_MASK;
    PCC0_PCC_PORTD |= PCC_CLKCFG_CGC_MASK;
    PCC0_PCC_PORTE |= PCC_CLKCFG_CGC_MASK;
    
    /* GPIO */
    PORTA->PCR[14] |= PORT_PCR_MUX(1);
    FGPIO0->PDDR |= (1<<14);
    
    volatile int i;
    i = 1;
//    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
//    printf("HelloWorld!\r\n");
//    shell_init();
    while(1)
    {
        FGPIO0->PTOR |= (1<<14);
   //  shell_main_loop("SHELL>>");
    }
}


void HardFault_Handler(void)
{
    
}

