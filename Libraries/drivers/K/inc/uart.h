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
	 
//! @defgroup CH_Periph_Driver
//! @{

//! @defgroup UART
//! @brief UART driver modules
//! @{
	 
	 
//! @addtogroup UART_Exported_Types
//! @{


#ifdef UART_USE_STDIO
#define UART_printf(fmt,args...)    printf (fmt ,##args)
#endif

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



typedef enum
{
    kUartParityDisabled = 0x0,  /*!< parity disabled */
    kUartParityEven     = 0x2,  /*!< parity enabled, type even, bit setting: PE|PT = 10 */
    kUartParityOdd      = 0x3,  /*!< parity enabled, type odd,  bit setting: PE|PT = 11 */
} UART_ParityMode_Type;


typedef enum
{
    kUartOneStopBit = 0,  /*!< one stop bit */
    kUartTwoStopBit = 1,  /*!< two stop bits */
} UART_StopBit_Type;


typedef enum
{
    kUART_ITDMA_Disable,      //!< Disable Interrupt and DMA
    kUART_IT_TxBTC,           //!< Byte Transfer Complete Interrupt for Tx
    kUART_DMA_TxBTC,          //!< DMA Trigger On Byte Transfer Complete for Tx
    kUART_IT_RxBTC,           //!< Byte Transfer Complete Interrupt for Rx
    kUART_DMA_RxBTC,          //!< DMA Trigger On Byte Transfer Complete for Rx
}UART_ITDMAConfig_Type;

//!< UART Init type
typedef struct
{
    uint8_t   instance;
    uint32_t  baudrate;
    uint8_t   parityMode;
    uint8_t   stopBitCount;
} UART_InitTypeDef;
     
//!< UART CallBack Type
typedef void (*UART_CallBackType)(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag);

//!< API functions
uint8_t UART_QuickInit(uint32_t UARTxMAP, uint32_t baudrate);
void UART_Init(UART_InitTypeDef * UART_InitStruct);
int UART_printf(const char *format,...);
uint8_t UART_ReadByte(uint8_t instance, uint8_t *ch);
void UART_WriteByte(uint8_t instance, uint8_t ch);
void UART_CallbackInstall(uint8_t instance, UART_CallBackType AppCBFun);
void UART_ITDMAConfig(uint8_t instance, UART_ITDMAConfig_Type config, FunctionalState newState);

//! @}

#ifdef __cplusplus
}
#endif

#endif

//! @}

//! @}

