#include "isr.h"





void PIT0_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 0);
  //  PIT_ClearITPendingBit(PIT0);
}

void PIT1_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 1);
  //  PIT_ClearITPendingBit(PIT1);
}

void PIT2_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 2);
   // PIT_ClearITPendingBit(PIT2);
}

void PIT3_IRQHandler(void)
{
    UART_printf("PIT%d ISR\r\n", 3);
   // PIT_ClearITPendingBit(PIT3);
}

void Watchdog_IRQHandler(void)
{
  //  WDOG_ClearAllITPendingBit();
  //  UART_printf("Watchdog_IRQHandler\r\n");
}



