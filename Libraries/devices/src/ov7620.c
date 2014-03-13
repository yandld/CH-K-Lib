#include "ov7620.h"
#include "gpio.h"
#include "board.h"
#include "dma.h"
#include "sram.h"

static const uint32_t PORT_DataAddressTable[] = 
{
    (uint32_t)&PTA->PDIR,
    (uint32_t)&PTB->PDIR,
    (uint32_t)&PTC->PDIR,
    (uint32_t)&PTD->PDIR,
    (uint32_t)&PTE->PDIR,
};

uint8_t CCDBufferPool[OV7620_W*OV7620_H];
//uint8_t * CCDBufferPool = SRAM_START_ADDRESS;
uint8_t * CCDBuffer[OV7620_H];
static OV7620_CallBackType OV7620_CallBackTable[1] = {NULL};
static uint32_t gHREF_RecievedCnt;
//包括场中断和 行中断
static void OV7620_ISR(uint32_t pinArray)
{
    uint16_t i,j;
    if(pinArray & (1<<BOARD_OV7620_VSYNC_PIN))
    {
        if(gHREF_RecievedCnt >= OV7620_H)
        {
            DMA_CancelTransfer(HW_DMA_CH2);
            GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_Disable);
            GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_Disable);
            if(OV7620_CallBackTable[0])
            {
                OV7620_CallBackTable[0]();
            }
        }
        gHREF_RecievedCnt = 0;
        GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_RisingEdge);
        GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);
    }
    if(pinArray & (1<<BOARD_OV7620_HREF_PIN))
    {
        DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)CCDBuffer[gHREF_RecievedCnt]);
        DMA_StartTransfer(HW_DMA_CH2); 
        gHREF_RecievedCnt++;
    }
}

void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");
    printf("gHREF_RecievedCnt:%d\r\n", gHREF_RecievedCnt);
    while(1);
    
}

void OV7620_CallbackInstall(OV7620_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        OV7620_CallBackTable[0] = AppCBFun;
    }
}

void OV7620_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct1;
    uint32_t i;
    uint32_t err_cnt;
    for(i=0;i< OV7620_H;i++)
    {
        CCDBuffer[i] = &CCDBufferPool[i*OV7620_W];
    }
    //初始化
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(BOARD_OV7620_DATA_PORT, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV7620_ISR); 
    GPIO_CallbackInstall(BOARD_OV7620_HREF_PORT, OV7620_ISR); 
    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_RisingEdge);
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);
    GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge); //实际并没有用到
    
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = DMA_MUX1; //尽最大努力采集
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorByteTransferCount = OV7620_W;
    DMA_InitStruct1.majorTransferCount = 1;
    
    DMA_InitStruct1.sourceAddress = (uint32_t)PORT_DataAddressTable[BOARD_OV7620_DATA_PORT] + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sourceAddressMajorAdj = 0;
    DMA_InitStruct1.sourceAddressMinorAdj = 0;
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8;
    
    DMA_InitStruct1.destAddress = (uint32_t)CCDBuffer[0];
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 1;
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
    DMA_Init(&DMA_InitStruct1);
}
