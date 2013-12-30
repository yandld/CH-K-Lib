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

#include "common.h"

#define HW_UART0  (0x00U)  //!< UART0 is different from UART1 UART2, It's a low power UART port
#define HW_UART1  (0x01U)
#define HW_UART2  (0x02U)

//!< UART quickInit macro
#define UART0_RX_PE21_TX_PE20   (0xA920U)
#define UART0_RX_PA01_TX_PA02   (0x8280U)
#define UART0_RX_PA15_TX_PA14   (0x9CC0U)
#define UART0_RX_PB16_TX_PB17   (0xA0C8U)
#define UART0_RX_PD06_TX_PD07   (0x8CD8U)
#define UART1_RX_PE01_TX_PE00   (0x80e1U)
#define UART1_RX_PA18_TX_PA19   (0xa4c1U)
#define UART1_RX_PC03_TX_PC04   (0x86d1U)
#define UART2_RX_PE23_TX_PE22   (0xad22U)
#define UART2_RX_PD02_TX_PD03   (0x84daU)
#define UART2_RX_PD04_TX_PD05   (0x88daU)


//!< UART init type
typedef struct
{
    uint8_t   instance;
    uint32_t  baudrate;
    uint8_t   parityMode;
    uint8_t   stopBitCount;
    uint32_t UARTxMAP;
} UART_InitTypeDef;

typedef enum
{
    kUART_ParityDisabled = 0x0,  /*!< parity disabled */
    kUART_ParityEven     = 0x2,  /*!< parity enabled, type even, bit setting: PE|PT = 10 */
    kUART_ParityOdd      = 0x3,  /*!< parity enabled, type odd,  bit setting: PE|PT = 11 */
} UART_ParityMode_Type;


typedef enum
{
    kUART_OneStopBit = 0,  /*!< one stop bit */
    kUART_TwoStopBit = 1,  /*!< two stop bits */
} UART_StopBit_Type;


typedef enum
{
    kUART_ITDMA_Disable,      //!< Disable Interrupt and DMA
    kUART_IT_TxBTC,           //!< Byte Transfer Complete Interrupt for Tx
    kUART_DMA_TxBTC,          //!< DMA Trigger On Byte Transfer Complete for Tx
    kUART_IT_RxBTC,           //!< Byte Transfer Complete Interrupt for Rx
    kUART_DMA_RxBTC,          //!< DMA Trigger On Byte Transfer Complete for Rx
}UART_ITDMAConfig_Type;

//!< UART CallBack Type
typedef void (*UART_CallBackType)(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag);

//<! API functions
uint8_t UART_QuickInit(uint32_t UARTxMAP, uint32_t baudrate);
void UART_Init(UART_InitTypeDef * UART_InitStruct);
void UART_WriteByte(uint8_t instance, uint8_t ch);
uint8_t UART_ReadByte(uint8_t instance, uint8_t *ch);
void UART_ITDMAConfig(uint8_t instance, UART_ITDMAConfig_Type config, FunctionalState newState);
void UART_CallbackInstall(uint8_t instance, UART_CallBackType AppCBFun);


#ifdef __cplusplus
}
#endif

#endif
