#include "shell.h"
#include "uart.h"
#include "common.h"

static uint8_t instance;
static uint8_t gReceicedChar;

// 发送完成中断
static void UART_TxISR(uint8_t * pbyteToSend)
{
    static uint8_t ch;
    *pbyteToSend = gReceicedChar;
    UART_ITDMAConfig(instance, kUART_IT_Tx_Disable);
}

// 接收完成中断
static uint8_t UART_RxISR(uint8_t pbyteReceived)
{
    gReceicedChar = pbyteReceived;
    UART_ITDMAConfig(instance, kUART_IT_Tx);
    
}

int CMD_UART(int argc, char * const * argv)
{
    shell_printf("UART Test CMD\r\n");
    if(argc == 1)
    {
        instance = strtoul(argv[1], NULL, 0);
        shell_printf("UART instance is:%d\r\n", instance);
        //开启 发送完成 接收完成中断
        UART_CallbackTxInstall(instance, UART_TxISR);
        UART_CallbackRxInstall(instance, UART_RxISR);
        UART_ITDMAConfig(instance, kUART_IT_Tx);
        UART_ITDMAConfig(instance, kUART_IT_Rx);
        shell_printf("program will echo and will not return to shell mode\r\n");
        while(1);
    }
    return 0;
}

const cmd_tbl_t CommandFun_UART = 
{
    .name = "UART",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = CMD_UART,
    .usage = "UART <instance>",
    .complete = NULL,
    .help = NULL,
};

