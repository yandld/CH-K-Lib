#include <stdio.h>
#include <string.h>

#include "common.h"
#include "lpuart.h"

    


int main(void)
{
    DelayInit();
	  LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    while(1){ 
    printf("HelloWorld!\r\n");
    }

}


