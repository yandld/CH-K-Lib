#include "shell.h"
#include "uart.h"
#include "common.h"

static uint8_t instance;
static uint8_t sflag = false;
#define CMD_UART_MAP UART2_RX_PE23_TX_PE22

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
        if(byteReceived == 's')
        {
            sflag = true;
            UART_ITDMAConfig(instance, kUART_IT_RxBTC, DISABLE);
            UART_ITDMAConfig(instance, kUART_IT_TxBTC, DISABLE);
        }
        else
        {
            UART_ITDMAConfig(instance, kUART_IT_TxBTC, ENABLE);
            ch = byteReceived;  
        }
    }
}

int CMD_UART(int argc, char * const * argv)
{
    static uint8_t init;
    shell_printf("UART Test CMD\r\n");
    if(!init)
    {
        instance = UART_QuickInit(CMD_UART_MAP, 115200);
        init = 1;
    }
    if(argc == 1)
    {
        
    }
    if(argc == 2)
    {
        
        if(!strcmp(argv[1], "IT"))
        {
            shell_printf("UART IT Test:will echo input by TxIT\r\ntype 's' to stop\r\n");
            UART_CallbackInstall(instance, UART_ISR);  
            UART_ITDMAConfig(instance, kUART_IT_RxBTC, ENABLE);
            while(sflag == false);
            shell_printf("\r\nUART IT TEST FINISH\r\n");
            sflag = false;
        }
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

