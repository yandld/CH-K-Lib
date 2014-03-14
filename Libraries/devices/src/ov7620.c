#include "ov7620.h"
#include "gpio.h"
#include "board.h"
#include "dma.h"
#include "sram.h"

static const void* PORT_DataAddressTable[] = 
{
    (void*)&PTA->PDIR,
    (void*)&PTB->PDIR,
    (void*)&PTC->PDIR,
    (void*)&PTD->PDIR,
    (void*)&PTE->PDIR,
};


//状态机
#define TRANSFER_IN_PROCESS  (0x00)
#define USER_IN_PROCESS      (0x01)
#define NEXT_FRAME  (0x02)

static uint8_t status = TRANSFER_IN_PROCESS;  

//uint8_t CCDBufferPool[OV7620_W*OV7620_H];   //使用内部RAM
uint8_t * CCDBufferPool = SRAM_START_ADDRESS; //使用外部SRAM
uint8_t * CCDBuffer[OV7620_H];
static OV7620_CallBackType OV7620_CallBackTable[1] = {NULL};
//包括场中断和 行中断
static void OV7620_ISR(uint32_t pinArray)
{
    switch(status)
    {
        case TRANSFER_IN_PROCESS:
            if(DMA_IsTransferComplete(HW_DMA_CH2) == 0)
            {
                GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_Disable);
                if(OV7620_CallBackTable[0])
                {
                    OV7620_CallBackTable[0]();
                }
                status = NEXT_FRAME;
                GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);  
            }
            else
            {
                status = TRANSFER_IN_PROCESS;
            }
            break;
        case NEXT_FRAME:
            DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)CCDBuffer[0]);
            DMA_StartTransfer(HW_DMA_CH2); 
            status =  TRANSFER_IN_PROCESS;
            break;
        default:
            break;
    }
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
    //把开辟的内存池付给指针
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
    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_DMA_RisingEdge);
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);
  //  GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge); //实际并没有用到
    //初始化DMA
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorByteTransferCount = OV7620_W;
    DMA_InitStruct1.majorTransferCount = OV7620_H;
    
    DMA_InitStruct1.sourceAddress = (uint32_t)PORT_DataAddressTable[BOARD_OV7620_DATA_PORT] + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sourceAddressMajorAdj = 0;
    DMA_InitStruct1.sourceAddressMinorAdj = 0;
    DMA_InitStruct1.sourceDataWidth = kDMA_DataWidthBit_8;
    
    DMA_InitStruct1.destAddress = (uint32_t)CCDBuffer[0];
    DMA_InitStruct1.destAddressMajorAdj = 0;
    DMA_InitStruct1.destAddressMinorAdj = 1;
    DMA_InitStruct1.destDataWidth = kDMA_DataWidthBit_8;
    //初始化DMA
    DMA_Init(&DMA_InitStruct1);
    //开始传输
    DMA_StartTransfer(HW_DMA_CH2); 
}
