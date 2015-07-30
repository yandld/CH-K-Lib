#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "spi.h"
#include "dma.h"
#include "w25qxx.h"


#define DMA_SPI_TX_CH   0
#define DMA_SPI_RX_CH   1

static uint32_t gProbleCmd[] = 
{
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0x90),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (0 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
};




void SPI_xfer(uint32_t instance, uint32_t *buf_in, uint32_t *buf_out, uint32_t size)
{
    int i;
    
    /* format the send buffer */
    for(i=0; i<size; i++)
    {
        buf_in[i] |= SPI_PUSHR_CONT_MASK | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1);
    }
    buf_in[size-1] &= ~SPI_PUSHR_CONT_MASK;
    
    DMA_SetSourceAddress(DMA_SPI_TX_CH, (uint32_t)buf_in);
    DMA_SetDestAddress(DMA_SPI_RX_CH, (uint32_t)buf_out);
    DMA_SetMajorLoopCounter(DMA_SPI_TX_CH, size);
    DMA_SetMajorLoopCounter(DMA_SPI_RX_CH, size);
    
    DMA_EnableRequest(1);
    DMA_EnableRequest(0);
}

static void UART_SPI_Config(uint32_t instance)
{
    /* MOSI */
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    DMA_InitStruct1.chl = DMA_SPI_TX_CH;
    DMA_InitStruct1.chlTriggerSource = SPI2_TRAN_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 4;
    DMA_InitStruct1.majorLoopCnt = 0;
    
    DMA_InitStruct1.sAddr = NULL;
    DMA_InitStruct1.sLastAddrAdj = 0; 
    DMA_InitStruct1.sAddrOffset = 4;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_32;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)&SPI2->PUSHR; 
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 0;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_32;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    DMA_Init(&DMA_InitStruct1);
    
    /* SPI MISO */
    DMA_InitStruct1.chl = DMA_SPI_RX_CH;
    DMA_InitStruct1.chlTriggerSource = SPI2_REV_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 4;
    DMA_InitStruct1.majorLoopCnt = 0;
    
    DMA_InitStruct1.sAddr = (uint32_t)&SPI2->POPR;
    DMA_InitStruct1.sLastAddrAdj = 0; 
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_32;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = NULL; 
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 4;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_32;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    DMA_Init(&DMA_InitStruct1);
    
    
}


static int w25qxx_test(void)
{
    uint32_t i, block, buf_size,j;
    static uint8_t buf[4*1024];
    
    printf("spi flash id:0x%X\r\n", w25qxx_get_id());
    buf_size = sizeof(buf);
    
    block = 512;
    
    /* erase chip */
    printf("erase all chips...\r\n");
    w25qxx_erase_chip();
    printf("erase complete\r\n");
    
    for(i=0; i<block; i++)
    {
        printf("verify addr:0x%X(%d)...\r\n", i*buf_size, i);
        for(j=0;j<sizeof(buf);j++)
        {
            buf[j] = j % 0xFF;
        }
        w25qxx_write(i*block, buf, buf_size);
        memset(buf, buf_size, 0);
        w25qxx_read(i*block, buf, buf_size);
        
        /* varify */
        for(j=0;j<sizeof(buf);j++)
        {
            if(buf[j] != (j%0xFF))
            {
                printf("%d error\r\n", j);
            }
        }
    }
    return 0;
}



int main(void)
{
    int i;
    uint32_t rev_buf[32];
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("DMA UART transmit test\r\n");

    SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14, kSPI_CPOL0_CPHA0, 1*1000*1000);
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); /* SPI2_PCS1 */
    SPI_EnableTxFIFO(HW_SPI2, true);
    SPI_EnableRxFIFO(HW_SPI2, true);
    
    UART_SPI_Config(2);
    SPI_ITDMAConfig(HW_SPI2, kSPI_DMA_TFFF, true);
    SPI_ITDMAConfig(HW_SPI2, kSPI_DMA_RFDF, true);

    SPI_xfer(HW_SPI2, gProbleCmd, rev_buf, 6);
    
    DelayMs(100);
    i = DMA_GetMajorLoopCount(DMA_SPI_TX_CH);
    printf("cnt:%d\r\n", i);
    i = DMA_GetMajorLoopCount(DMA_SPI_RX_CH);
    printf("cnt:%d\r\n", i);
    for(i=0; i<ARRAY_SIZE(rev_buf); i++)
    {
        printf("0x%X  ", rev_buf[i]);
    }
    while(1);
    
    struct w25qxx_init_t init;
    
    
    if(w25qxx_init(&init))
    {
        printf("w25qxx device no found!\r\n");
    }
    else
    {
        printf("spi_flash found id:0x%X\r\n", w25qxx_get_id());
    }

    w25qxx_test();
    
    printf("w252qxx test complete\r\n");
    while(1)
    {
        /* 通过串口使用dma功能实现数据发送 */
    //   UART_SendWithDMA(HW_DMA_CH2, (const uint8_t*)String1, sizeof(String1));
        /* 等待DMA传输结束 */
     //   while(DMA_IsMajorLoopComplete(HW_DMA_CH2));
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(200);
    }
}


