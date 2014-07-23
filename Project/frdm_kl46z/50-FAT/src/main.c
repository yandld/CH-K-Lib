#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "shell.h"

 
int main(void)
{
    uint16_t ch;
    DelayInit();

    UART_QuickInit(UART2_RX_PE17_TX_PE16, 115200);
    printf("HelloWorld!\r\n");
    shell_init();
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}




