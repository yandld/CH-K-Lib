#include "gpio.h"
#include "common.h"
#include "uart.h"

/* 发送一串字符 */
static void UART_SendString(uint32_t instance, char * str)
{
    while(*str != '\0')
    {
        UART_WriteByte(instance, *str++);
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* 调试串口 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    /* 485 和 普通的TTL 串口 只是后级硬件不同 对于MCU来说，都是串口 */
    UART_QuickInit(UART5_RX_PE09_TX_PE08, 115200);
    printf("485 test! please connnect other 485 device\r\n");
    
    /* 向485总线发送一串字符 */
    UART_SendString(HW_UART5, "485 device test\r\n");
    uint16_t ch;
    while(1)
    {
        /* 将485总线的数据发回去 */
        if(UART_ReadByte(HW_UART5, &ch) == 0)
        {
            UART_WriteByte(HW_UART5, ch);
        }
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


