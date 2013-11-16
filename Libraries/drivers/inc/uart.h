/**
  ******************************************************************************
  * @file    uart.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.25
  * @brief   超核K60固件库 UART 串口 驱动库 头文件
  ******************************************************************************
  */
#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys.h"
	 
//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup UART
//! @brief UART driver modules
//! @{
	 
	 
//! @addtogroup UART_Exported_Types
//! @{
	 
//可使用的UART初始化结构
typedef enum
{
	UART0_RX_PA01_TX_PA02 = 0x8280U,                
	UART0_RX_PA14_TX_PA15 = 0x9CC0U,                
	UART0_RX_PB16_TX_PB17 = 0xA0C8U,                 
	UART0_RX_PD06_TX_PD07 = 0x8CD8U,   
	UART1_RX_PE00_TX_PE01 = 0x80E1U,
	UART1_RX_PC03_TX_PC04 = 0x86D1U,
	UART2_RX_PD02_TX_PD03 = 0x84DAU,
	UART3_RX_PB10_TX_PB11 = 0x94CBU,
	UART3_RX_PC16_TX_PC17 = 0xA0D3U,
	UART3_RX_PE04_TX_PE05 = 0x88E3U,
	UART4_RX_PE24_TX_PE25 = 0xB0E4U,
	UART4_RX_PC14_TX_PC15 = 0x9CD4U,
	UART0_USER_DEFINE = 0x0U,
	UART1_USER_DEFINE = 0x1U,
	UART2_USER_DEFINE = 0x2U,
	UART3_USER_DEFINE = 0x3U,
	UART4_USER_DEFINE = 0x4U,         
}UART_MapSelect_TypeDef;

//参数检查器														
#define IS_UART_ALL_PERIPH(PERIPH) (((PERIPH) == UART0) || \
                                    ((PERIPH) == UART1) || \
                                    ((PERIPH) == UART2) || \
                                    ((PERIPH) == UART3) || \
                                    ((PERIPH) == UART4))												
																
//串口初始化结构
typedef struct
{
  uint32_t UART_BaudRate;                    //!< UART baudrate
	UART_MapSelect_TypeDef UARTxMAP;           //!< UART Module and pinmux select
} UART_InitTypeDef;

#define IS_UART_ALL_PERIPH(PERIPH) (((PERIPH) == UART0) || \
                                    ((PERIPH) == UART1) || \
                                    ((PERIPH) == UART2) || \
                                    ((PERIPH) == UART3) || \
                                    ((PERIPH) == UART4))												
																
typedef enum
{
    kUART_IT_TDRE,                //!< Transmit Data Register Empty flag
    kUART_IT_TC,                  //!< Transmit Complete Flag
    kUART_IT_RDRF,                //!< Receive Data Register full flag
    kUART_IT_IDLE,                //!< Idle line flag
}UART_ITSelect_TypeDef;

//DMA命令
typedef enum
{
	kUART_DMAReq_TX,                //!< UART DMA TX Request
	kUART_DMAReq_RX,                //!< UART DMA RX Request
}UART_DMAReq_Select_TypeDef;

//! @}

/*
//缓冲区最大值
#define MAX_TX_BUF_SIZE     128

typedef struct
{
	uint8_t TxBuf[MAX_TX_BUF_SIZE];
	uint8_t Length;
	uint8_t Offset;
	uint8_t IsComplete;
	uint16_t MaxBufferSize;
}UART_TxSendTypeDef;
//串口中断发送结构
extern UART_TxSendTypeDef UART_TxIntStruct1;
*/

extern UART_Type* UART_DebugPort;

//! @defgroup UART_Exported_Functions
//! @{

void UART_SendByte(UART_Type* UARTx,uint8_t Data);
uint8_t UART_ReceiveByte_Async(UART_Type *UARTx,uint8_t *ch);
uint8_t UART_ReceiveByte(UART_Type *UARTx,uint8_t *ch);
void UART_SendDataInt(UART_Type* UARTx,uint8_t* DataBuf,uint8_t Len);
void UART_Init(UART_InitTypeDef* UART_InitStruct);
void DisplayCPUInfo(void);
void UART_SendDataIntProcess(UART_Type* UARTx);
void UART_DMACmd(UART_Type* UARTx, UART_DMAReq_Select_TypeDef UART_DMAReq, FunctionalState NewState);
void UART_DebugPortInit(UART_MapSelect_TypeDef UARTxMAP,uint32_t UART_BaudRate);
void UART_ITConfig(UART_Type* UARTx, UART_ITSelect_TypeDef UART_IT, FunctionalState NewState);
ITStatus UART_GetITStatus(UART_Type* UARTx, UART_ITSelect_TypeDef UART_IT);
int UART_printf(const char *format,...);

//! @}

#ifdef __cplusplus
}
#endif

#endif

//! @}

//! @}
