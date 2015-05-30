#include "trans.h"
#include <string.h>
#include "dma.h"
#include "uart.h"
#include "common.h"



int uart_dma_init(uint8_t dmaChl, uint32_t uartInstance)
{
    DMA_InitTypeDef DMAInitStruct;

    DMAInitStruct.chl = HW_DMA_CH0;
    DMAInitStruct.chlTriggerSource = UART0_TRAN_DMAREQ;
    DMAInitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

    DMAInitStruct.sAddr = 0;
    DMAInitStruct.sAddrIsInc = true;
    DMAInitStruct.sDataWidth = kDMA_DataWidthBit_8;
    DMAInitStruct.sMod = kDMA_ModuloDisable;

    DMAInitStruct.dAddr = (uint32_t)&UART0->D;
    DMAInitStruct.dAddrIsInc = false;
    DMAInitStruct.dDataWidth = kDMA_DataWidthBit_8;
    DMAInitStruct.dMod = kDMA_ModuloDisable;
    DMA_Init(&DMAInitStruct);
    
    DMA_EnableAutoDisableRequest(HW_DMA_CH0, true);
    UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx, true);
    return 0;
}

int uart_dma_send(uint8_t* buf, uint32_t len)
{
    uint32_t remain;
    
//    remain = DMA_GetTransferByteCnt(HW_DMA_CH0);
//    if(remain)
//    {
//        return 1;
//    }
    
    DMA_CancelTransfer(HW_DMA_CH0);
    DMA_SetSourceAddress(HW_DMA_CH0, (uint32_t)buf);
    DMA_SetTransferByteCnt(HW_DMA_CH0, len);
    DMA_EnableRequest(HW_DMA_CH0);
    return 0;
}



