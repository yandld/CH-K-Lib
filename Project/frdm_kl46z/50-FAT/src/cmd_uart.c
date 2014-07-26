#include "shell.h"
#include "uart.h"

void UART_ISR(uint16_t data)
{
    printf("%s %c\r\n", __func__, data);
}


static int DoUART(int argc, char * const argv[])
{
    uint32_t instance;
    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    UART_CallbackRxInstall(instance, UART_ISR);
    UART_ITDMAConfig(instance, kUART_IT_Rx, true);
}

SHELL_EXPORT_CMD(DoUART, uart , uart test)
    

