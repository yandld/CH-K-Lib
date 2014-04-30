#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"

#define DMA_SEND_CH             HW_DMA_CH1
#define DMA_REV_CH              HW_DMA_CH2

/* 接收缓冲区 */
static uint8_t UART_RxBuffer[16];
/* UART 模块数据寄存器 */
static const void* UART_DataPortAddrTable[] = 
{
    (void*)&UART0->D,
    (void*)&UART1->D,
    (void*)&UART2->D,
    (void*)&UART3->D,
    (void*)&UART4->D,
    (void*)&UART5->D,    
};
/* UART 发送触发源编号 */
static const uint32_t UART_SendDMATriggerSourceTable[] = 
{
    UART0_TRAN_DMAREQ,
    UART1_TRAN_DMAREQ,
    UART2_TRAN_DMAREQ,
    UART3_TRAN_DMAREQ,
    UART4_TRAN_DMAREQ,
    UART5_TRAN_DMAREQ,
};

static const uint32_t UART_RevDMATriggerSourceTable[] = 
{
    UART0_REV_DMAREQ,
    UART1_REV_DMAREQ,
    UART2_REV_DMAREQ,
    UART3_REV_DMAREQ,
    UART4_REV_DMAREQ,
    UART5_REV_DMAREQ,
};

/* DMA 发送函数 */
static uint32_t UART_SendWithDMA(uint32_t dmaChl, const uint8_t *buf, uint32_t size)
{
    DMA_SetSourceAddress(dmaChl, (uint32_t)buf);
    DMA_SetMajorLoopCount(dmaChl, size);
    /* 启动传输 */
    DMA_EnableRequest(dmaChl);
    return 0;
}
/* DMA 串口发送 配置 */
static void UART_DMASendInit(uint32_t uartInstnace, uint8_t dmaChl)
{
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    DMA_InitStruct1.chl = dmaChl;
    DMA_InitStruct1.chlTriggerSource = UART_SendDMATriggerSourceTable[uartInstnace];
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = 0;
    
    DMA_InitStruct1.sAddr = NULL;
    DMA_InitStruct1.sLastAddrAdj = 0; 
    DMA_InitStruct1.sAddrOffset = 1;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)UART_DataPortAddrTable[uartInstnace]; 
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 0;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    DMA_Init(&DMA_InitStruct1);
}
/* DMA 串口接收 配置 */
static void UART_DMARevInit(uint32_t uartInstnace, uint8_t dmaChl)
{
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    DMA_InitStruct1.chl = dmaChl;
    DMA_InitStruct1.chlTriggerSource = UART_RevDMATriggerSourceTable[uartInstnace];
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = 1;
    
    DMA_InitStruct1.sAddr = (uint32_t)&UART0->D;
    DMA_InitStruct1.sLastAddrAdj = 0; 
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = NULL; 
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 0;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    DMA_Init(&DMA_InitStruct1); 
}

/* DMA 中断函数 */
void DMA_ISR(void)
{
    static uint8_t ch;
    ch = UART_RxBuffer[0];
    /*DMA 发送 */
    UART_SendWithDMA(DMA_SEND_CH, &ch, 1);
    /* 重新开启 DMA接收 */
    DMA_SetMajorLoopCount(DMA_REV_CH, 1);
    DMA_EnableRequest(DMA_REV_CH);
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DMA UART transmit test\r\n");
    
    /* 配置DMA 打开UART_Tx_DMA功能 */
    UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx);
    UART_DMASendInit(HW_UART0, DMA_SEND_CH);
    
    /* 配置DMA 打开UART_Rx_DMA功能 */
    UART_ITDMAConfig(HW_UART0, kUART_DMA_Rx);
    UART_DMARevInit(HW_UART0, DMA_REV_CH);
    
    /* 打开 DMA 中断 */
    DMA_CallbackInstall(DMA_REV_CH, DMA_ISR);
    DMA_ITConfig(DMA_REV_CH, kDMA_IT_Major);
    
    /* 设置接收缓冲区 使能DMA接收 */
    DMA_SetDestAddress(DMA_REV_CH, (uint32_t)UART_RxBuffer);
    DMA_EnableRequest(DMA_REV_CH);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


