#ifndef __LPUART_H__
#define __LPUART_H__

#include "common.h"

#define HW_LPUART0  (0x00U)

//!< LPUART QuickInit macro
#define UART0_RX_PE21_TX_PE20  (0xA920U)
#define UART0_RX_PA01_TX_PA02  (0x8280U)
#define UART0_RX_PA15_TX_PA14  (0x9CC0U)
#define UART0_RX_PB16_TX_PB17  (0xA0C8U)
#define UART0_RX_PD06_TX_PD07  (0x8CD8U)



typedef enum
{
    kUartParityDisabled = 0x0,  /*!< parity disabled */
    kUartParityEven     = 0x2,  /*!< parity enabled, type even, bit setting: PE|PT = 10 */
    kUartParityOdd      = 0x3,  /*!< parity enabled, type odd,  bit setting: PE|PT = 11 */
} LPUART_ParityMode_Type;


typedef enum
{
    kUartOneStopBit = 0,  /*!< one stop bit */
    kUartTwoStopBit = 1,  /*!< two stop bits */
} LPUART_StopBit_Type;


typedef enum
{
    kLPUART_ITDMA_Disable,      //!< Disable Interrupt and DMA
    kLPUART_IT_TxBTC,           //!< Byte Transfer Complete Interrupt for Tx
    kLPUART_DMA_TxBTC,          //!< DMA Trigger On Byte Transfer Complete for Tx
    kLPUART_IT_RxBTC,           //!< Byte Transfer Complete Interrupt for Rx
    kLPUART_DMA_RxBTC,          //!< DMA Trigger On Byte Transfer Complete for Rx
}LPUART_ITDMAConfig_Type;

//串口初始化结构
typedef struct
{
    uint8_t   instance;
    uint32_t  baudrate;
    uint8_t   parityMode;
    uint8_t   stopBitCount;
    uint32_t UARTxMAP;           //初始化结构
} LPUART_InitTypeDef;


//!< I2C CallBack Type
typedef void (*LPUART_CallBackType)(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag);

//!< API functions
uint8_t LPUART_QuickInit(uint32_t LPUARTxMAP, uint32_t baudrate);
void LPUART_Init(LPUART_InitTypeDef * LPUART_InitStruct);
void LPUART_ITDMAConfig(uint8_t instance, LPUART_ITDMAConfig_Type config, FunctionalState newState);
void LPUART_CallbackInstall(uint8_t instance, LPUART_CallBackType AppCBFun);






#endif

