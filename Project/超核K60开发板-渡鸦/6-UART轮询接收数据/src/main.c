#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

 
int main(void)
{
    uint8_t ch;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    /* 初始化一个模块的一般模式: 初始化模块本身->根据芯片手册 初始化对应的复用引脚->使用模块 */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.baudrate = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* 初始化串口0对应的引脚 D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /* 打印芯片信息 */
    printf(" type any character whitch will echo...\r\n");
    
    while(1)
    {
        /* 不停的查询 串口接收的状态 一旦接收成功 返回0 发送回接收到的数据 实现回环测试*/
        if(UART_ReadByte(HW_UART0, &ch) == 0)
        {
            UART_WriteByte(HW_UART0, ch);
        }
    }
}


