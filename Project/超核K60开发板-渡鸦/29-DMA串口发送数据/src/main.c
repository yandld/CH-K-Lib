#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：DMA串口发送数据
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用串口发送数据，仅仅是使用了DMA功能，处理速度更快
*/

/* DMA 发送函数 */
/* 0 此次发送成功 else 此次发送失败 */
static uint32_t UART0_SendWithDMA(uint32_t dmaChl, uint8_t *buf, uint32_t size)
{
    DMA_InitTypeDef DMA_InitStruct1;
    if(DMA_IsTransferComplete(dmaChl))
    {
        /* DMA 通道正忙 */
        return 1;
    }
    DMA_InitStruct1.chl = dmaChl;  /* 使用1通道 */
    DMA_InitStruct1.chlTriggerSource = UART0_TRAN_DMAREQ; /* 串口完成传输一帧后触发 */
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal; /* 普通模式 不是周期触发模式 */
    DMA_InitStruct1.minorByteTransferCount = 1; //一次触发传输一字节
    DMA_InitStruct1.majorTransferCount = size;  //总共传输的字节数
    
    DMA_InitStruct1.sourceAddress = (uint32_t)buf; /*源地址 */
    DMA_InitStruct1.sourceAddressMajorAdj = 0; 
    DMA_InitStruct1.sourceAddressMinorAdj = 1; //地址偏移1个字节
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8; /* 8位数据位宽 */
    
    DMA_InitStruct1.destAddress = (uint32_t)&UART0->D; 
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 0;  //目标地址不偏移
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8; //8字节数据位宽
    DMA_Init(&DMA_InitStruct1);
    /* 启动传输 */
    DMA_StartTransfer(dmaChl);
    return 0;
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("DMA UART transmit test\r\n");
    
    /* 打开UART0 DMA发送使能 */
    UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx);
    
    while(1)
    {
        //通过串口使用dma功能实现数据发送
        UART0_SendWithDMA(HW_DMA_CH0, "This string is send via DMA\r\n", 29);
        /* 等待DMA传输结束 */
        while(DMA_IsTransferComplete(HW_DMA_CH0) == 1);
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(50);
    }
}


