#ifndef __BOOTLOADER_UTIL_H__
#define __BOOTLOADER_UTIL_H__

#include <common.h>
#include "uart.h"

#define CMD_SERIALPORT      0xFD        /* 来自串口的命令 */
#define CMD_CHIPINFO        0x80
#define CMD_APP_INFO        0x81
#define CMD_TRANS_DATA      0x82
#define CMD_VERIFICATION    0x83
#define CMD_APP_CHECK       0x84
#define CMD_SOFT_RESET      0x85


/* 运行参数 */
typedef struct
{
    uint32_t write_addr;		        /* 写地址 */
    volatile uint32_t usart_timeout;	/* 超时记数器 */
    uint32_t app_length;		        /* 应用程序长度 */
    volatile uint32_t delayms_cnt;	    /* 毫秒延时计数器 */
    uint8_t  currentPkgNo;              /* 当前包号 */
    uint8_t  total_pkg;			        /* 总包长 */
    volatile uint32_t wdt_timeout;	    /* 看门狗超时计数器 */
    uint8_t  op_state;
    uint8_t  bootloaderFlg;
    uint8_t  retryCnt;
} RunType_t;


extern RunType_t M_Control;
extern UART_Type *pUARTx;


extern void Fn_RxProcData(const uint8_t buf);
extern void Fn_SendResponse(uint8_t* content, uint8_t cipherFlg,  uint16_t len);




#endif
