/**
  ******************************************************************************
  * @file    uart.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "common.h"
	 

#ifdef UART_USE_STDIO
#define UART_printf(fmt,args...)    printf (fmt ,##args)
#endif

//!< hardware instances
#define HW_UART0  (0x00U)
#define HW_UART1  (0x01U)
#define HW_UART2  (0x02U)
#define HW_UART3  (0x03U)
#define HW_UART4  (0x04U)
#define HW_UART5  (0x05U)

//!< UART QuickInit macro                            
#define UART1_RX_PE01_TX_PE00   (0x80E1U)
#define UART0_RX_PF17_TX_PF18   (0xA528U)
#define UART3_RX_PE05_TX_PE04   (0x88E3U)
#define UART5_RX_PF19_TX_PF20   (0xA72DU)
#define UART5_RX_PE09_TX_PE08   (0x90E5U)
#define UART2_RX_PE17_TX_PE16   (0xA0E2U)
#define UART4_RX_PE25_TX_PE24   (0xB0E4U)
#define UART0_RX_PA01_TX_PA02   (0x8280U)
#define UART0_RX_PA15_TX_PA14   (0x9CC0U)
#define UART3_RX_PB10_TX_PB11   (0x94CBU)
#define UART0_RX_PB16_TX_PB17   (0xA0C8U)
#define UART1_RX_PC03_TX_PC04   (0x86D1U)
#define UART4_RX_PC14_TX_PC15   (0x9CD4U)
#define UART3_RX_PC16_TX_PC17   (0xA0D3U)
#define UART2_RX_PD02_TX_PD03   (0x84DAU)
#define UART0_RX_PD06_TX_PD07   (0x8CD8U)
#define UART2_RX_PF13_TX_PF14   (0x9B2AU)
#define UART5_RX_PD08_TX_PD09   (0x90DDU)



//!< UART paritySelect
typedef enum
{
    kUartParityDisabled = 0x0,  //!< parity disabled
    kUartParityEven     = 0x2,  //!< parity enabled, type even, bit setting: PE|PT = 10
    kUartParityOdd      = 0x3,  //!< parity enabled, type odd,  bit setting: PE|PT = 11
} UART_ParityMode_Type;

//!< UART Stopbit select
typedef enum
{
    kUartOneStopBit = 0,  //!< one stop bit
    kUartTwoStopBit = 1,  //!< two stop bits
} UART_StopBit_Type;

//!< interrupt and DMA select
typedef enum
{
    kUART_IT_Tx_Disable,       //!< Disable Tx Interrupt
    kUART_IT_Rx_Disable,       //!< Disable Rx Interrupt
    kUART_DMA_Tx_Disable,      //!< Disable Tx DMA
    kUART_DMA_Rx_Disable,      //!< Disable Rx DMA
    kUART_IT_Tx,           //!< Byte Transfer Complete Interrupt for Tx
    kUART_DMA_Tx,          //!< DMA Trigger On Byte Transfer Complete for Tx
    kUART_IT_Rx,           //!< Byte Transfer Complete Interrupt for Rx
    kUART_DMA_Rx,          //!< DMA Trigger On Byte Transfer Complete for Rx
}UART_ITDMAConfig_Type;

//!< UART Init type
typedef struct
{
    uint8_t   instance;       //!< UART instance
    uint32_t  baudrate;       //!< UART baudrate
    uint8_t   parityMode;     //!< UART parity mode
    uint8_t   stopBitCount;   //!< UART stopbit
}UART_InitTypeDef;

//!< UART CallBack Type
typedef void (*UART_CallBackTxType)(uint8_t * pbyteToSend);
typedef void (*UART_CallBackRxType)(uint8_t byteReceived);

//!< API functions
uint8_t UART_QuickInit(uint32_t UARTxMAP, uint32_t baudrate);
void UART_Init(UART_InitTypeDef * UART_InitStruct);
int UART_printf(const char *format,...);
uint8_t UART_ReadByte(uint8_t instance, uint8_t *ch);
void UART_WriteByte(uint8_t instance, uint8_t ch);
void UART_CallbackTxInstall(uint8_t instance, UART_CallBackTxType AppCBFun);
void UART_CallbackRxInstall(uint8_t instance, UART_CallBackRxType AppCBFun);
void UART_ITDMAConfig(uint8_t instance, UART_ITDMAConfig_Type config);
#ifdef UART_USE_STDIO
int printf(const char *fmt, ...);
#endif


//!< param check
#define IS_UART_ALL_INSTANCE(INSTANCE)  (INSTANCE < ARRAY_SIZE(UART_InstanceTable))

#ifdef __cplusplus
}
#endif

#endif

