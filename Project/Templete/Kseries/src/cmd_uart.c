#include "shell.h"
#include "uart.h"
#include "common.h"

static uint8_t instance;

static void UART_ISR(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag)
{
    static uint8_t ch;
    // 发送完成中断
    if(flag == kUART_IT_TxBTC)
    {
        *pbyteToSend = ch;
        UART_ITDMAConfig(instance, kUART_IT_TxBTC, DISABLE);
    }
    //接收字节中断
    if(flag == kUART_IT_RxBTC)
    {
        ch = byteReceived;
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

