#include "shell.h"
#include "lpuart.h"
#include "common.h"

static uint8_t instance;
uint8_t sflag = false;

static void LPUART_ISR(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag)
{
    static uint8_t ch;
    if(flag == kLPUART_IT_TxBTC)
    {
        *pbyteToSend = ch;
        LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_TxBTC, DISABLE);
    }
    if(flag == kLPUART_IT_RxBTC)
    {
        if(byteReceived == 's')
        {
            sflag = true;
            LPUART_ITDMAConfig(instance, kLPUART_IT_RxBTC, DISABLE);
            LPUART_ITDMAConfig(instance, kLPUART_IT_TxBTC, DISABLE);
        }
        else
        {
            LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_TxBTC, ENABLE);
            ch = byteReceived;  
        }
    }
}

int CMD_LPUART(int argc, char * const * argv)
{
    uint32_t i;
    static uint8_t init;
    shell_printf("LPUART Test CMD\r\n");
    if(!init)
    {
        instance = LPUART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
        init = 1;
    }
    if(argc == 1)
    {
        
    }
    if(argc == 2)
    {
        
        if(!strcmp(argv[1], "IT"))
        {
            shell_printf("LPUART IT Test:will echo input by TxIT\r\ntype 's' to stop\r\n");
            LPUART_CallbackInstall(instance, LPUART_ISR);  
            LPUART_ITDMAConfig(instance, kLPUART_IT_RxBTC, ENABLE);
            while(sflag == false);
            shell_printf("\r\nLPUART IT TEST FINISH\r\n");
            sflag = false;
        }
    }
    return 0;
}

const cmd_tbl_t CommandFun_LPUART = 
{
    .name = "LPUART",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = CMD_LPUART,
    .usage = "LPUART",
    .complete = NULL,
    .help = "\r\n" 
};

