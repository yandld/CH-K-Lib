#include "shell/shell.h"
#include "uart.h"
#include "common.h"

static uint8_t instance;

static void UART_ISR(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag)
{
    static uint8_t ch;
    if(flag == kUART_IT_TxBTC)
    {
        *pbyteToSend = ch;
        UART_ITDMAConfig(instance, kUART_IT_TxBTC, DISABLE);
    }
    if(flag == kUART_IT_RxBTC)
    {
        byteReceived = ch;
        UART_ITDMAConfig(instance, kUART_IT_TxBTC, ENABLE);
    }
}

int CMD_UART(int argc, char * const * argv)
{
    shell_printf("UART Test CMD\r\n");
    if(argc == 2)
    {
        instance = strtoul(argv[1], NULL, 0);
        shell_printf("UART instance is:%d\r\n", instance);
        UART_CallbackInstall(instance, UART_ISR);  
        UART_ITDMAConfig(instance, kUART_IT_RxBTC, ENABLE);   
    }
    return 0;
}

const cmd_tbl_t CommandFun_UART = 
{
    .name = "UART",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = CMD_UART,
    .usage = "UART <IT>",
    .complete = NULL,
    .help = NULL,
};

