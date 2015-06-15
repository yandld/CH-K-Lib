#include <stdio.h>
#include <string.h>

#include "common.h"
#include "lpuart.h"

    


int main(void)
{
	  uint32_t clock;
    DelayInit();
	  LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    /* ¥Ú”° ±÷”∆µ¬  */
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("core clock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("bus clock:%dHz\r\n", clock);   

}


