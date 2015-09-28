#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include <stdint.h>
#include "uart.h"
#include "common.h"

/* 
    @fn:    BootloaderInit
    @bref:  初始化Bootloader
    @params: 
            UARTxMAP ： 选择对应的串口及此串口对应的IO口配置，见MAIN函数
            baudrate:   串口波特率 最大值试过2000000(2Mbps, 更高的没条件测试)
            delayMs:    代表等待这么长时间如果没有执行下载操作，则跳转到程序区执行
    @return: 1 代表初始化成功，0 代表初始化失败
*/
extern uint8_t BootloaderInit(UART_Type *uartch, uint32_t baudrate, uint32_t delayMs);


/* 
    @fn:    BootloaderProc
    @bref:  Bootloader的处理函数
    @params: 无
    @return: 无
*/
extern void BootloaderProc(void);



#endif
