#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"

#include <string.h>

static uint8_t SourceBuffer[64];
static uint8_t DestBuffer[64];

int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DMA memcpy test\r\n");
    
    /* 一次触发后 执行 minorByteTransferCount 完成后 叫做一次Minor Loop */
    /* majorTransferCount 个 Minor Loop 循环执行后 DMA结束传输 */
    
    /* 写入测试数据 并且将被测试数据清空*/
    memset(SourceBuffer, 'T', ARRAY_SIZE(SourceBuffer));
    memset(DestBuffer, 0, ARRAY_SIZE(DestBuffer));
    
    DMA_InitTypeDef DMA_InitStruct1;
    DMA_InitStruct1.chl = HW_DMA_CH0;  /* 使用0通道 */
    DMA_InitStruct1.chlTriggerSource = DMA_MUX1; /*尽最大努力传输 */
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal; /* 普通模式 不是周期触发模式 */
    DMA_InitStruct1.minorByteTransferCount = ARRAY_SIZE(SourceBuffer); /*每次触发传输*/
    DMA_InitStruct1.majorTransferCount = 1; /* 最大可以被触发 1次 */
    
    DMA_InitStruct1.sourceAddress = (uint32_t)SourceBuffer; /*源地址 */
    DMA_InitStruct1.sourceAddressMajorAdj = 0; /* 每次触发传输Minor Loop后源地址 偏移量 */
    DMA_InitStruct1.sourceAddressMinorAdj = 1;  /* 每次触发传输Major Loop后源地址 偏移量 */
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8; /* 8位数据位宽 */
    
    DMA_InitStruct1.destAddress = (uint32_t)DestBuffer; /* 目的地址 */
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 1; /*每次传输后 目的地址+1 */
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
    DMA_Init(&DMA_InitStruct1);
    /* 启动传输 */
    DMA_StartTransfer(HW_DMA_CH0);
    /* 等待DMA传输结束 */
    while(DMA_IsTransferComplete(HW_DMA_CH0));
    for(i=0;i<ARRAY_SIZE(DestBuffer);i++)
    {
        /* 如果拷贝不成功 */
        if(DestBuffer[i] != SourceBuffer[i])
        {
            printf("DMA test error:%d\r\n", i);
            while(1);
        }
    }
    printf("DMA memcpy test OK!");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


