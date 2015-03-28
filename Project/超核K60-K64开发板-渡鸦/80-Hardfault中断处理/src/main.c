#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"


 
int main(void)
{
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);

    printf("Hardfault Interrupt test!\r\n");
    
    /* close PORTA gate clock */
    SIM->SCGC5 &= ~SIM_SCGC5_PORTA_MASK;
    
    
    printf("operate PORTA without open it's clock gate, that will trigger hardfault!\r\n");
    
    /* R/W PORTA, this will trigger hardfault */
    PORTA->PCR[0] = 0;
    
    while(1)
    {
        /* …¡À∏–°µ∆ */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


void HardFault_Handler(void)
{
    printf("hardFault handler interrupt!\r\n");
    while(1);
}


