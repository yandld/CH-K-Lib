#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "shell.h"

 
int main(void)
{
    DelayInit();

    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    printf("HelloWorld!\r\n");
    shell_init();
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}




