#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"
#include "ili9320.h"
#include "dma.h"

#include "ov7725.h"
#include "image_display.h"
#include "i2c.h"

/* 请将I2C.H中的 I2C_GPIO_SIM 改为 1 */

// 改变图像大小
//0: 80x60
//1: 160x120
//2: 240x180
#define IMAGE_SIZE  0

#if (IMAGE_SIZE  ==  0)
#define OV7725_W    (80)
#define OV7725_H    (60)

#elif (IMAGE_SIZE == 1)
#define OV7725_W    (160)
#define OV7725_H    (120)

#elif (IMAGE_SIZE == 2)
#define OV7725_W    (240)
#define OV7725_H    (180)

#else
#error "Image Size Not Support!"
#endif

uint8_t gCCD_RAM[(OV7725_H)*(OV7725_W/8)];
uint8_t img[OV7725_H*OV7725_W]; 


typedef enum
{
    IMG_FINISH,
    IMG_FAIL,
    IMG_GATHER,
    IMG_START,
} OV_Status;

volatile OV_Status gStatus;

/* 引脚定义 PCLK VSYNC HREF 接到同一个PORT上 */
#define BOARD_OV7725_PCLK_PORT      HW_GPIOA
#define BOARD_OV7725_PCLK_PIN       (7)
#define BOARD_OV7725_VSYNC_PORT     HW_GPIOA
#define BOARD_OV7725_VSYNC_PIN      (16)
#define BOARD_OV7725_HREF_PORT      HW_GPIOA
#define BOARD_OV7725_HREF_PIN       (17)
/* 
摄像头数据引脚PTA8-PTA15 只能填入 0 8 16三个值 
0 :PTA0-PTA7
8 :PTA8-PTA15
16:PTA16-PTA24
*/
#define BOARD_OV7725_DATA_OFFSET    (8) 



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

//航中断和长中断都使用PTA中断
void OV_ISR(uint32_t index)
{
    if(index & (1 << BOARD_OV7725_VSYNC_PIN))
    {
        if(gStatus == IMG_START)                   
        {
            gStatus = IMG_GATHER;
            DMA_SetDestAddress (HW_DMA_CH2,(uint32_t)gCCD_RAM);
            PORTA ->ISFR |= 1<<BOARD_OV7725_PCLK_PIN;
            DMA_EnableRequest (HW_DMA_CH2 );
            PORTA ->ISFR |= 1 <<  BOARD_OV7725_PCLK_PIN;
        }
    }
}


void img_extract(uint8_t *dst, uint8_t *src, uint32_t srclen)
{
    uint8_t colour[2] = {0, 1}; 
   
    uint8_t tmpsrc;
    while(srclen --)
    {
        tmpsrc = *src++;
        *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
    }
}

#define  DMA_IRQ_CLEAN(DMA_CHn) DMA0->INT|=(DMA_INT_INT0_MASK<<DMA_CHn) 

void ov7725_DMA(void )
{
    gStatus = IMG_FINISH ;
    DMA_DisableRequest (HW_DMA_CH2 );
    DMA_IRQ_CLEAN (HW_DMA_CH2);
}

void camera_get_image()
{
    gStatus = IMG_START;
    printf("IMG_START--1\r\n");
    PORTA->ISFR |= (1 << BOARD_OV7725_VSYNC_PIN);
    NVIC_EnableIRQ(PORTA_IRQn);
   
    while(gStatus != IMG_FINISH)
    {
        
    }
}

void SerialDispImage(uint8_t* srcData)
{
    uint32_t x, y;
    for(y = 0; y < OV7725_H; y++)
    {
        for(x = 0; x < (OV7725_W); x++)
        {
            if(srcData[y*OV7725_W+x])
            {
                ili9320_write_pixel(x, y, BLACK);
            }
            else
            {
                ili9320_write_pixel(x, y, WHITE);
            }
        }			
    }
}


int main(void)
{
    uint32_t i;
    DelayInit();
    /* 打印串口及小灯 */
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("OV7725 test\r\n");
    
    //初始化GUI
    CHGUI_Init();
    GUI_printf(0, 0, "OV7725 test");
    
    //检测摄像头
    if(SCCB_Init(I2C0_SCL_PB00_SDA_PB01))
    {
        printf("no ov7725device found!\r\n");
        while(1);
    }
    printf("OV7620 setup complete\r\n");
    
    
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    
    /* 场中断 像素中断 */
    GPIO_QuickInit(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_Mode_IPD);
    
    /* install callback */
    GPIO_CallbackInstall(BOARD_OV7725_VSYNC_PORT, OV_ISR);
   
    GPIO_ITDMAConfig(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
    
    /* 初始化数据端口 */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOA, BOARD_OV7725_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    
    //DMA配置
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = ((OV7725_W/8)*OV7725_H);
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTA->PDIR + BOARD_OV7725_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)gCCD_RAM;
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;

    /* initialize DMA moudle */
    DMA_Init(&DMA_InitStruct1);
    
    DMA_ITConfig (HW_DMA_CH2 ,kDMA_IT_Major ,true );
    DMA_CallbackInstall (HW_DMA_CH2 ,ov7725_DMA);
    DMA_DisableRequest (HW_DMA_CH2 );
    
    while(1)
    {
       camera_get_image();
       img_extract(img, gCCD_RAM ,OV7725_H*(OV7725_W/8));  
       SerialDispImage(img);
    }
}
