#include "gpio.h"
#include "common.h"
#include "i2c.h"
#include "dma.h"
#include "uart.h"
#include "ssd1306.h"


#include "ov7725.h"
//#include "image_display.h"
#include "i2c.h"


// size
//0: 80x60
//1: 160x120
//2: 240x180
#define IMAGE_SIZE  0

#if (IMAGE_SIZE  ==  0)
#define OV7620_W    (80)
#define OV7620_H    (60)

#elif (IMAGE_SIZE == 1)
#define OV7620_W    (160)
#define OV7620_H    (120)

#elif (IMAGE_SIZE == 2)
#define OV7620_W    (240)
#define OV7620_H    (180)

#else
#error "Image Size Not Support!"
#endif


uint8_t gCCD_RAM[(OV7620_H)*((OV7620_W/8)+1)];


uint8_t * gpHREF[OV7620_H+1];


#define BOARD_OV7620_PCLK_PORT      HW_GPIOA
#define BOARD_OV7620_PCLK_PIN       (7)
#define BOARD_OV7620_VSYNC_PORT     HW_GPIOA
#define BOARD_OV7620_VSYNC_PIN      (16)
#define BOARD_OV7620_HREF_PORT      HW_GPIOA
#define BOARD_OV7620_HREF_PIN       (17)

/* 
DATA PORT pffset
0 :PTA0-PTA7
8 :PTA8-PTA15
16:PTA16-PTA24
*/
#define BOARD_OV7620_DATA_OFFSET    (8) 


typedef enum
{
    TRANSFER_IN_PROCESS,
    NEXT_FRAME,
}OV7620_Status;

static void UserApp(uint32_t vcount);


void SerialDispCCDImage(int xSize, int ySize, uint8_t** ppData)
{
    int x,y;
    
    for(y = 0; y < ySize; y++)
    {
        for(x = 1; x < (xSize/8)+1; x++)
        {
            printf("%d",(ppData[y][x]>>7) & 0x01);
            printf("%d",(ppData[y][x]>>6) & 0x01);
            printf("%d",(ppData[y][x]>>5) & 0x01);
            printf("%d",(ppData[y][x]>>4) & 0x01);
            printf("%d",(ppData[y][x]>>3) & 0x01);
            printf("%d",(ppData[y][x]>>2) & 0x01);
            printf("%d",(ppData[y][x]>>1) & 0x01);
            printf("%d",(ppData[y][x]>>0) & 0x01);
            if(x == xSize/8)
                printf("\r\n");   
        }
        if(y==ySize -1)
        {
            printf("                                                                                ");
            printf("\r\n");  
        }				
    }
}


static void UserApp(uint32_t vcount)
{
    SerialDispCCDImage(OV7620_W, OV7620_H, gpHREF);
}


int SCCB_Init(uint32_t I2C_MAP)
{
    int r;
    uint32_t instance;
    instance = I2C_QuickInit(I2C_MAP, 50*1000);
    r = ov7725_probe(instance);
    if(r)
    {
        return 1;
    }
    r = ov7725_set_image_size(IMAGE_SIZE);
    if(r)
    {
        printf("OV7725 set image error\r\n");
        return 1;
    }
    return 0;
}

// 行中断和场中断都使用PTA
void OV_ISR(uint32_t index)
{
    static uint8_t status = TRANSFER_IN_PROCESS;
    static uint32_t h_counter, v_counter;
   // uint32_t i;
    
    /* 行中断 */
    if(index & (1 << BOARD_OV7620_HREF_PIN))
    {
        DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)gpHREF[h_counter++]);
        //i = DMA_GetMajorLoopCount(HW_DMA_CH2);
        DMA_SetMajorLoopCounter(HW_DMA_CH2, (OV7620_W/8)+1);
        DMA_EnableRequest(HW_DMA_CH2);
        
        return;
    }
    /* 长中断 */
    if(index & (1 << BOARD_OV7620_VSYNC_PIN))
    {
        GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, false);
        GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, false);
        switch(status)
        {
            case TRANSFER_IN_PROCESS: 
                    UserApp(v_counter++);
                    //printf("i:%d %d\r\n", h_counter, i);
                    status = NEXT_FRAME;
                    h_counter = 0;

                break;
            case NEXT_FRAME: // waiting for next transfer 
                status =  TRANSFER_IN_PROCESS;
                break;
            default:
                break;
        }
        GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
        GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, true);
        PORTA->ISFR = 0xFFFFFFFF;
        h_counter = 0;
        return;
    }
}


int main(void)
{
    uint32_t i;
    DelayInit();
    DelayMs(10);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOC, 2, kGPIO_Mode_OPP);

    
    printf("Hello K60 OV7725!\r\n");
    
    //INIT OV7725
    if(SCCB_Init(I2C0_SCL_PB00_SDA_PB01))
    {
        printf("no ov7725device found!\r\n");
        while(1);
    }
    printf("OV7620 setup complete\r\n");
    
    //RAM 
    for(i=0; i<OV7620_H+1; i++)
    {
        gpHREF[i] = (uint8_t*)&gCCD_RAM[i*OV7620_W/8];
    }
    
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    
    /* HREF VSYNC PCLK */
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
    
    /* install callback */
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV_ISR);
   
    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
    
    /* DATA PORT */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOA, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    
    //DMA
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = ((OV7620_W/8) +1);
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTA->PDIR + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)gpHREF[0];
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;

    /* initialize DMA moudle */
    DMA_Init(&DMA_InitStruct1);

    while(1)
    {
        PEout(0) = !PEout(0);
        PCout(2) = !PCout(2);
        DelayMs(500);
    }
}


