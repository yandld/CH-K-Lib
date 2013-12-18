#ifndef __LPUART_H__
#define __LPUART_H__

#include <stdint.h>


#define HW_LPUART0  (0x00U)

typedef enum
{
    kUartParityDisabled = 0x0,  /*!< parity disabled */
    kUartParityEven     = 0x2,  /*!< parity enabled, type even, bit setting: PE|PT = 10 */
    kUartParityOdd      = 0x3,  /*!< parity enabled, type odd,  bit setting: PE|PT = 11 */
} UART_ParityMode_Type;

//串口初始化结构
typedef struct
{
    uint8_t   instance;
    uint32_t  baudrate;
    uint8_t   parityMode;
		uint8_t   stopBitCount;
		uint8_t   bitCountPerChar;
    uint32_t UARTxMAP;           //初始化结构
} LPUART_InitTypeDef;



#endif

