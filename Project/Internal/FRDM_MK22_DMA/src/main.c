#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "spi.h"
#include "dma.h"


/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */


/*K22的DMA请求源号与K64有些不同 需重新定义一下*/
#define SPI0_RECEIVE_DMAREQ         14																		
#define SPI0_SEND_DMAREQ        15
#define SPI1_SEND_RECEIVE_DMAREQ    16
/* 定义使用的DMA通道号 */
#define DMA_SEND_CH             HW_DMA_CH1
#define DMA_REV_CH              HW_DMA_CH2

static uint32_t SPI_sendBuffer[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
static uint32_t SPI_receiveBuffer[16];

/* SPI 发送触发源编号 */
static const uint32_t SPI_SendDMATriggerSourceTable[] = 
{
		SPI0_SEND_DMAREQ,
		SPI1_SEND_RECEIVE_DMAREQ,
}; 
/* SPI 接收触发源编号 */
static const uint32_t SPI_RevDMATriggerSourceTable[] = 
{
		SPI0_RECEIVE_DMAREQ,
		SPI1_SEND_RECEIVE_DMAREQ,
}; 
/* SPI模块发送寄存器*/
static const void* SPI_PUSHPortAddrTable[] = 
{
    (void*)&SPI0->PUSHR,
    (void*)&SPI1->PUSHR,  
};
/* SPI模块接收寄存器*/
static const void* SPI_DataPortAddrTable[] = 
{
    (void*)&SPI0->POPR,
    (void*)&SPI1->POPR,  
}; 

/**
 * @brief  设置 DMA 源地址
 * @param[in]  chl  DMA 通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * \param[in]  * txBuf  源地址
 * @retval None
 */
void DMA_SetSAddr(uint8_t chl, uint32_t * txBuf)
{

		DMA0->TCD[chl].SADDR = (uint32_t)txBuf;
}
/**
 * @brief   设置 DMA 目标地址
 * @param[in]  chl  DMA 通道号
 *         			@arg HW_DMA_CH0
 *         			@arg HW_DMA_CH1
 *         			@arg HW_DMA_CH2
 *         			@arg HW_DMA_CH3
 * \param[in]  * rxBuf 目标地址
 * @retval None
 */
void DMA_SetDAddr(uint8_t chl, uint32_t * rxBuf)
{

		DMA0->TCD[chl].DADDR = (uint32_t)rxBuf;
}


void DMA_ISR(void);

/* SPI发送 DMA配置 */
static void SPI_DMASendInit(uint32_t SPIInstnace, uint8_t dmaChl, uint32_t * txBuf)
{
		DMA_InitTypeDef DMA_InitStruct1 = {0};																							/*清空初始化结构体*/
    DMA_InitStruct1.chl = dmaChl;																												/*选择DMA通道*/
    DMA_InitStruct1.chlTriggerSource = SPI_SendDMATriggerSourceTable[SPIInstnace];			/*选择触发源SPI0发送*/
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;											/*选择触发方式*/
    DMA_InitStruct1.minorLoopByteCnt = 4;																								/*次循环 每次DMA请求传输的字节数*/
    DMA_InitStruct1.majorLoopCnt = 16;																									/*主循环 这里是DMA请求次数*/
																																												/*这里源数组 uint32_t SPI_sendBuffer[16] 故每次传输4个字节 总共传输16次*/
    DMA_InitStruct1.sAddr = (uint32_t)txBuf;																						/*源地址*/
    DMA_InitStruct1.sLastAddrAdj = 0; 																									/*所有major loop完成后源地址偏移量*/
    DMA_InitStruct1.sAddrOffset = 4;																										/*每次minor loop完成后源地址偏移量*/
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_32;																	/*源数据地址宽度*/
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;																					/*模值设置*/
    
    DMA_InitStruct1.dAddr = (uint32_t)SPI_PUSHPortAddrTable[SPIInstnace]; 							/*目标地址*/
    DMA_InitStruct1.dLastAddrAdj = 0;																										/*所有major loop完成后目标地址偏移量*/
    DMA_InitStruct1.dAddrOffset = 0;																										/*每次minor loop完成后目标地址偏移量*/
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_32;																	/*目标地址数据宽度*/
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;																					/*模值设置*/
    DMA_Init(&DMA_InitStruct1);																													/*初始化DMA模块*/
		DMA_EnableAutoDisableRequest(dmaChl, true);																					/*所有major loop完成后自动关闭DMA请求*/
		DMA_CallbackInstall(dmaChl, DMA_ISR);																								/*注册DMA1中断回调函数*/
		DMA_ITConfig(dmaChl, kDMA_IT_Major, true);																					/*使能DMA中断*/
    DMA_EnableRequest(dmaChl);																													/*使能DMA*/
}

/*SPI 接收 DMA配置*/
static void SPI_DMAReceiveInit(uint32_t SPIInstnace, uint8_t dmaChl, uint32_t * rxBuf)
{
		DMA_InitTypeDef DMA_InitStruct1 = {0};																							/*清空初始化结构体*/
    DMA_InitStruct1.chl = dmaChl;																												/*选择DMA通道*/
    DMA_InitStruct1.chlTriggerSource = SPI_RevDMATriggerSourceTable[SPIInstnace];				/*选择触发源SPI0接收*/
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;											/*选择触发方式*/
    DMA_InitStruct1.minorLoopByteCnt = 4;																								/*次循环 每次DMA请求传输的字节数*/
    DMA_InitStruct1.majorLoopCnt = 1;																										/*主循环 这里是DMA请求次数*/
    
    DMA_InitStruct1.sAddr = (uint32_t)SPI_DataPortAddrTable[SPIInstnace];								/*源地址*/
    DMA_InitStruct1.sLastAddrAdj = 0; 																									/*所有major loop完成后源地址偏移量*/
    DMA_InitStruct1.sAddrOffset = 0;																										/*每次minor loop完成后源地址偏移量*/
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_32;																	/*源数据地址宽度*/
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;																					/*模值设置*/
    
    DMA_InitStruct1.dAddr = (uint32_t)rxBuf; 																						/*目标地址*/
    DMA_InitStruct1.dLastAddrAdj = 0;																										/*所有major loop完成后目标地址偏移量*/
    DMA_InitStruct1.dAddrOffset = 4;																										/*每次minor loop完成后目标地址偏移量*/
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_32;																	/*目标地址数据宽度*/
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;																					/*模值设置*/
    DMA_Init(&DMA_InitStruct1);																													/*初始化DMA模块*/
		DMA_EnableAutoDisableRequest(dmaChl, false);																				/*所有major loop完成后不关闭DMA请求 继续等待触发*/
    DMA_EnableRequest(dmaChl);																													/*使能DMA*/
}


uint8_t DMA_SendFinish = 0;


int main(void)
{	
    DelayInit();

		GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_OPP);																				/*初始化PTD4 SPI片选信号*/
/*--------------------------SPI0 初始化------------------------------*/	
		SPI_InitTypeDef SPI_InitStruct1;
		SPI_InitStruct1.baudrate = 1000000;																									/*波特率1000k*/
		SPI_InitStruct1.frameFormat = kSPI_CPOL0_CPHA0;																			/*无数据传输时SCK为低 无相位差*/
		SPI_InitStruct1.dataSize = 8;																												/*数据宽度*/
		SPI_InitStruct1.instance = HW_SPI0;																									/*选择SPI0*/
		SPI_InitStruct1.mode = kSPI_Master;																									/*主模式*/
		SPI_InitStruct1.bitOrder = kSPI_MSB;																								/*先发最高位*/
		SPI_InitStruct1.ctar = HW_CTAR0;																										/*使用0号控制寄存器*/
		
		PORT_PinMuxConfig(HW_GPIOD, 1, kPinAlt2);																						/*引脚配置 PTD1:SCK PTD2:MOSI PTD3:MISO*/
		PORT_PinMuxConfig(HW_GPIOD, 2, kPinAlt2);
		PORT_PinMuxConfig(HW_GPIOD, 3, kPinAlt2);		
		SPI_Init(&SPI_InitStruct1);																													/*初始化SPI模块*/
		SPI_ITDMAConfig(HW_SPI0,kSPI_DMA_TFFF, true);																				/*使能发送DMA请求*/
		GPIO_ResetBit(HW_GPIOD,4);																													/*片选拉低*/
		SPI_DMASendInit(HW_SPI0, DMA_SEND_CH, SPI_sendBuffer);															/* SPI发送 DMA配置 */
	
	

		SPI_ITDMAConfig(HW_SPI0,kSPI_DMA_RFDF, true);																				/*使能接收DMA请求*/
		SPI_DMAReceiveInit(HW_SPI0, DMA_REV_CH, SPI_receiveBuffer);													/*SPI 接收 DMA配置*/

/*----------------------------------------------------------------------*/
		
    UART_QuickInit(UART1_RX_PE01_TX_PE00, 115200);
    printf("CoreClock:%dHz\r\n", GetClock(kCoreClock));
    printf("BusClock:%dHz\r\n", GetClock(kBusClock));

		
    while(1)
    {					
				if(DMA_SendFinish == 1)
				{
						DMA_SendFinish = 0;
						DelayMs(200);

						uint8_t i;
						for(i=0; i<16; i++)																													/*清空数组 再次接收*/
						{
								SPI_receiveBuffer[i] = 0;
						}
						DMA_SetMajorLoopCounter(DMA_SEND_CH,16);																		/*重载major loop值*/
						DMA_SetSAddr(DMA_SEND_CH, SPI_sendBuffer);																	/*重新设置发送源地址*/
						DMA_SetDAddr(DMA_REV_CH, SPI_receiveBuffer);																/*重设接收地址*/
						DMA_EnableRequest(DMA_SEND_CH);																							/*使能发送通道DMA请求*/					
				}
    }
}
//DMA1中断回调函数
void DMA_ISR(void)
{
		DMA_SendFinish = 1;																																	/*DMA传输完成标志位*/
		printf("DMA INT \r\n");
}


