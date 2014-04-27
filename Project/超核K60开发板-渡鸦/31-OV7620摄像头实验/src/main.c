#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "sram.h"
#include "ili9320.h"
#include "dma.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：OV7620摄像头实验
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：在液晶屏上显示图像，需要0V7620摄像头
*/
#define OV7620_W    (320) // 每行有多少像素
#define OV7620_H    (240) // 高度有多少行

//uint8_t CCDBufferPool[OV7620_W*OV7620_H];   //使用内部RAM
volatile uint8_t * CCDBufferPool = SRAM_START_ADDRESS; //使用外部SRAM

/* CCD内存池 */
uint8_t * CCDBuffer[OV7620_H];
/* 引脚定义 */
#define BOARD_OV7620_PCLK_PORT      HW_GPIOA
#define BOARD_OV7620_PCLK_PIN       (7)   //摄像头时钟引脚
#define BOARD_OV7620_VSYNC_PORT     HW_GPIOA
#define BOARD_OV7620_VSYNC_PIN      (16) //摄像头像素中断
#define BOARD_OV7620_HREF_PORT      HW_GPIOA
#define BOARD_OV7620_HREF_PIN       (17) //摄像头行中断
#define BOARD_OV7620_DATA_OFFSET    (8) /* 摄像头数据引脚PTA8-PTA15 只能为 0 8 16 24 */

/* 状态机定义 */
typedef enum
{
    TRANSFER_IN_PROCESS, //数据在处理
    NEXT_FRAME,          //下一帧数据
}OV7620_Status;

static void UserApp(void);

/* 摄像头场中断处理函数 */
void OV7620_ISR(uint32_t pinArray)
{

    static uint8_t status = TRANSFER_IN_PROCESS;  
    switch(status)
    {
        case TRANSFER_IN_PROCESS: /* 正在传输 */
            /* 查看DMA是否完成传送 */
            if(DMA_IsTransferComplete(HW_DMA_CH2) == 0)
            {
                GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_Disable);
                /* 开始处理用户 函数 */
                UserApp();
                /* 用户函数处理完毕 */
                status = NEXT_FRAME;
                /* 处理完用户函数后 继续开启场中断 准备下一场数据到达 */
                GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);  
            }
            else
            {
                status = TRANSFER_IN_PROCESS;
            }
            break;
        case NEXT_FRAME:
            /* 传输完成 复位buffer地址 开始下一场传输 */
            DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)CCDBuffer[0]);
            DMA_StartTransfer(HW_DMA_CH2); 
            status =  TRANSFER_IN_PROCESS;
            break;
        default:
            break;
    }
}

static uint16_t RGB2COLOR(uint8_t RR,uint8_t GG,uint8_t BB)
{
  return (((RR/8)<<11)+((GG/8)<<6)+BB/8); 
}

/* 接收完成一场后 用户处理函数 */
static void UserApp(void)
{
    uint32_t i,j;
    static uint32_t cnt;
    printf("SYNC cnt:%d\r\n", cnt++); 
    for(i=0;i<OV7620_H;i++)
    {
        for(j=0;j<OV7620_W;j++)
        {
            ili9320_write_pixel(OV7620_H - i, OV7620_W- j, RGB2COLOR(CCDBuffer[i][j], CCDBuffer[i][j], CCDBuffer[i][j]));
        }
    } 
}

//ov7620摄像头初始化配置
static void OV7620_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    uint32_t i;
    //把开辟的内存池付给指针
    for(i=0;i< OV7620_H;i++)
    {
        CCDBuffer[i] = (uint8_t *) &CCDBufferPool[i*OV7620_W];
    }
    /* 场中断  行中断 像素中断 */
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
    /* 初始化数据端口 */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOA, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    /* 安装回调函数 */
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV7620_ISR);
    /* 行中断配置为DMA触发 */
    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_DMA_RisingEdge);
    /* 场中断配置为中断触发 */
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);
    /* 像素中断 */
  //  GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge); //实际并没有用到
    /* 初始化DMA */
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = OV7620_W;
    DMA_InitStruct1.majorLoopCnt = OV7620_H;
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTA->PDIR + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    
    DMA_InitStruct1.dAddr = (uint32_t)CCDBuffer[0];
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;

    DMA_Init(&DMA_InitStruct1);
    /* 开始传输 */
    DMA_StartTransfer(HW_DMA_CH2); 
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("OV7620 test\r\n");
    
    ili9320_init();
    SRAM_Init();
    /* 摄像头速度非常快 把FLexbus 总线速度调到最高 */
    SIM->CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV3_MASK;
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(2);
    OV7620_Init();

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


