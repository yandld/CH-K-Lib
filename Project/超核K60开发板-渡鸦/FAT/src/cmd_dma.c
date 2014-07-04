#include "shell.h"
#include "dma.h"
#include "common.h"
#include "systick.h"

static void DMA_ISR(void)
{
    static uint32_t cnt;
    shell_printf("DMA INT ENTER%d\r\n", cnt++);
}
static uint8_t SourceBuffer[64];
static uint8_t DestBuffer[64];
int CMD_DMA(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("DMA TEST CMD\r\n");
    for(i=0;i<ARRAY_SIZE(SourceBuffer);i++)
    {
        SourceBuffer[i] = i+1;
    }
    
    DMA_InitTypeDef DMA_InitStruct1 = {0};
		# if 0
    DMA_InitStruct1.chl = 1;
    DMA_InitStruct1.chlTriggerSource = DMA_MUX1;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorByteTransferCount = 10;
    DMA_InitStruct1.majorTransferCount = 1;
    
    DMA_InitStruct1.sourceAddress = (uint32_t)SourceBuffer;
    DMA_InitStruct1.sourceAddressMajorAdj = -5;
    DMA_InitStruct1.sourceAddressMinorAdj = 1;
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8;
    
    DMA_InitStruct1.destAddress = (uint32_t)DestBuffer;
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 1;
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
		#endif
    DMA_Init(&DMA_InitStruct1);
    DMA_EnableRequest(1);
    DelayMs(1);
    DMA_EnableRequest(1);
    DelayMs(1);
    for(i=0;i<ARRAY_SIZE(DestBuffer);i++)
    {
        printf("[%d]:%d\r\n", i, DestBuffer[i]);
    }
    return 0;
}

const cmd_tbl_t CommandFun_DMA = 
{
    .name = "DMA",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_DMA,
    .usage = "DMA TEST",
    .complete = NULL,
    .help = "\r\n"
};
