#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"
#include "ili9320.h"
#include "dma.h"
#include "i2c.h"
#include "ov7725.h"


#define OV7620_W    (80)
#define OV7620_H    (60)


uint8_t CCDBufferPool[(OV7620_H+1)*(OV7620_W/8)];   //使用内部RAM

/* CCD内存池 */
uint8_t * CCDBuffer[OV7620_H+1];

/* 引脚定义 */
#define BOARD_OV7620_PCLK_PORT      HW_GPIOA
#define BOARD_OV7620_PCLK_PIN       (7)
#define BOARD_OV7620_VSYNC_PORT     HW_GPIOA
#define BOARD_OV7620_VSYNC_PIN      (16)
#define BOARD_OV7620_HREF_PORT      HW_GPIOA
#define BOARD_OV7620_HREF_PIN       (17)
#define BOARD_OV7620_DATA_OFFSET    (8) /* 摄像头数据引脚PTA8-PTA15 只能为 0 8 16 24 */

/* 状态机定义 */
typedef enum
{
    TRANSFER_IN_PROCESS, //数据在处理
    NEXT_FRAME,          //下一帧数据
}OV7620_Status;

static void UserApp(void);


static void lcd_disp_bin(int x, int y, uint8_t data)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        if((data >> i) & 1)
        {
            ili9320_write_pixel(x+8-i, y, 0x0000);
        }
        else
        {
            ili9320_write_pixel(x+8-i, y, 0xFFFF);
        }
    }
}

/* 接收完成一场后 用户处理函数 */
static void UserApp(void)
{
    uint32_t x, y;
    for(y = 0; y < OV7620_H; y++)
    {
        for(x = 1; x < (OV7620_W/8)+1; x++)
        {
            lcd_disp_bin(x*8, y, CCDBuffer[y][x]);
        }
    }
    DelayMs(20);
}


int SCCB_Init(void)
{
    int r;
    uint32_t instance;
    instance = I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    r = ov7725_probe(instance);
    if(r)
    {
        return 1;
    }
    ov7725_set_image_size(H_80_W_60);
    return 0;
}

void OV_ISR(uint32_t index)
{
    static uint8_t status = TRANSFER_IN_PROCESS;
    static uint32_t h_counter, v_counter;
    static uint32_t i;
    
    /* 行中断 */
    if(index & (1 << BOARD_OV7620_HREF_PIN))
    {
        DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)CCDBuffer[h_counter++]);
        i = DMA_GetMajorLoopCount(HW_DMA_CH2);
        DMA_SetMajorLoopCounter(HW_DMA_CH2, 20);
        DMA_EnableRequest(HW_DMA_CH2); 
        return;
    }
    
    /* 场中断 */
    if(index & (1 << BOARD_OV7620_VSYNC_PIN))
    {
        DisableInterrupts();
        switch(status)
        {
            case TRANSFER_IN_PROCESS:
                    UserApp();
                    printf("i:%d %d\r\n", h_counter, i);
                    status = NEXT_FRAME;
                    h_counter = 0;

                break;
            case NEXT_FRAME:
                status =  TRANSFER_IN_PROCESS;
                h_counter = 0;
            default:
                break;
        }
        EnableInterrupts(); 
        h_counter = 0;
        return;
    }
}



int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("OV7725 test\r\n");
    
    if(SCCB_Init())
    {
        printf("no device found!\r\n");
    }
    ili9320_init();
   // SRAM_Init();
   // lcd_disp_bin(0,10,0x01);
   // lcd_disp_bin(24,10,0x41);
    for(i = 0; i < OV7620_H+1; i++)
    {
        CCDBuffer[i] = (uint8_t*)&CCDBufferPool[i*OV7620_W/8];
    }
    
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    
    /* 场中断  行中断 像素中断 */
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
    
    /* install callback */
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV_ISR);
    

    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
    
    /* 初始化数据端口 */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOA, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = (OV7620_W/8);
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTA->PDIR + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)CCDBuffer[0];
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;

    /* initialize DMA moudle */
    DMA_Init(&DMA_InitStruct1);
    
    while(1)
    {
        
    }
}


