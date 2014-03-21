#include "shell.h"
#include "uart.h"
#include "common.h"
#include "board.h"
#include "dma.h"

static uint8_t instance;
static uint8_t gReceicedChar;
static char TestBuffer[] = "UART DMA TEST\r\n";


static void DMA_ISR(void)
{
    shell_printf("ENTER DMA INT\r\n");
}

// 发送完成中断
static void UART_TxISR(uint8_t * pbyteToSend)
{
    *pbyteToSend = gReceicedChar;
    //发送完成后关闭发送完成中断 否则总是会进入发送完成中断
    UART_ITDMAConfig(instance, kUART_IT_Tx_Disable);
}

// 接收完成中断
static void UART_RxISR(uint8_t pbyteReceived)
{
    gReceicedChar = pbyteReceived;
    //开启发送中断
    UART_ITDMAConfig(instance, kUART_IT_Tx);
}

int CMD_UART(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("UART Test CMD\r\n");
    instance = UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    shell_printf("UART instance is:%d\r\n", instance);
    if(argc == 2 && (!strcmp(argv[1],"IT")))
    {
        //开启 发送完成 接收完成中断
        UART_CallbackTxInstall(instance, UART_TxISR);
        UART_CallbackRxInstall(instance, UART_RxISR);
        UART_ITDMAConfig(instance, kUART_IT_Tx);
        UART_ITDMAConfig(instance, kUART_IT_Rx);
        shell_printf("program will echo and will not return to shell mode\r\n");
        while(1);
    }
    if(argc == 2 && (!strcmp(argv[1],"DMA")))
    {
        DMA_InitTypeDef DMA_InitStruct1;
        DMA_InitStruct1.chl = 1;
        DMA_InitStruct1.chlTriggerSource = UART0_TRAN_DMAREQ;
        DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
        DMA_InitStruct1.minorByteTransferCount = 1;
        DMA_InitStruct1.majorTransferCount = ARRAY_SIZE(TestBuffer);
        
        DMA_InitStruct1.sourceAddress = (uint32_t)TestBuffer;
        DMA_InitStruct1.sourceAddressMajorAdj = -ARRAY_SIZE(TestBuffer);
        DMA_InitStruct1.sourceAddressMinorAdj = 1;
        DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8;
        
        DMA_InitStruct1.destAddress = (uint32_t)&UART0->D;
        DMA_InitStruct1.destAddressMajorAdj = 0;
        DMA_InitStruct1.destAddressMinorAdj = 0;
        DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
        DMA_Init(&DMA_InitStruct1);
        DMA_CallbackInstall(1, DMA_ISR);
        DMA_ITConfig(1, kDMA_IT_Major);
        DelayMs(2); //等待串口空闲
        UART_ITDMAConfig(instance, kUART_DMA_Tx); //开启UART0 DMA 发送
        for(i=0;i<10;i++)
        {
            DMA_StartTransfer(1);
            while(DMA_IsTransferComplete(1) == 1);
        }
        UART_ITDMAConfig(instance, kUART_DMA_Tx_Disable);
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

