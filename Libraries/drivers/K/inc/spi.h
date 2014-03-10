/**
  ******************************************************************************
  * @file    spi.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "common.h"
  
#define HW_SPI0     (0x00)
#define HW_SPI1     (0x01)
#define HW_SPI2     (0x02)
  
//!< CPHA
typedef enum
{
    kSPI_CPHA_1Edge,
    kSPI_CPHA_2Edge,
} SPI_CPHA_Type;

//!< CPOL
typedef enum
{
    kSPI_CPOL_InactiveLow,
    kSPI_CPOL_InactiveHigh,
} SPI_CPOL_Type;

typedef enum
{
    kSPI_Master,
    kSPI_Slave,
} SPI_Mode_Type;

//!< SPI 波特率选择
typedef enum
{
    kSPI_BaudrateDiv_2,
    kSPI_BaudrateDiv_4,
    kSPI_BaudrateDiv_6,
    kSPI_BaudrateDiv_8,
    kSPI_BaudrateDiv_16,
    kSPI_BaudrateDiv_32,
    kSPI_BaudrateDiv_64,
    kSPI_BaudrateDiv_128,
    kSPI_BaudrateDiv_256,
    kSPI_BaudrateDiv_512,
    kSPI_BaudrateDiv_1024,
}SPI_BaudrateDiv_Type;

//!< interrupt and DMA select
typedef enum
{
    kSPI_IT_TCF_Disable,    //!< 禁止TCF中断(TCF:传输一次完成)
    kSPI_DMA_TCF_Disable,   //!< 禁止TCF标准位中断
    kSPI_IT_TCF,            //!< SPI传输一次完成中断使能
    kSPI_DMA_TCF,           //!< SPI传输一次完成中断启动DMA
}SPI_ITDMAConfig_Type;


#define kSPI_PCS_ReturnInactive   (0x00)      //!< 传输完成后CS信号保持片选中状态
#define kSPI_PCS_KeepAsserted  (0x01)      //!< 传输完成后CS信号保持未选中状态

//!< 初始化结构
typedef struct
{
    uint32_t                instance;               //!< 模块号
    SPI_CPHA_Type           CPHA;                   //!< 时钟相位
    SPI_CPOL_Type           CPOL;                   //!< 时钟极性
	SPI_Mode_Type           mode;                   //!< 主从模式
    SPI_BaudrateDiv_Type    baudrateDivSelect;      //!< 波特率分频选择   
    uint8_t                 dataSizeInBit;          //!< 每帧数据有多少位 通常为8或16
}SPI_InitTypeDef;


//!< API functions
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct);
uint16_t SPI_ReadWriteByte(uint32_t instance, uint16_t data, uint16_t CSn, uint16_t csState);







