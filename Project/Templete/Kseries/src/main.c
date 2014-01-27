#include "clock.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "cpuidy.h"
#include "stdio.h"
#include "common.h"

int main(void)
{
    DelayInit();
    //初始化 PC3 PC4 为UART1引脚 波特率115200
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    UART_printf("HelloWorld\r\n");
      
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOD, 0, kGPIO_Mode_OPP);
    while(1)
    {
        DelayMs(500);
        UART_printf("I am CHK\r\n");
        GPIO_ToggleBit(HW_GPIOA, 1);
        GPIO_ToggleBit(HW_GPIOD, 0);
    }
}



#ifdef USE_FULL_ASSERT
void assert_failed(char * file, uint32_t line)
{
	//断言失败检测
	while(1);
}
#endif
