#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "cpuidy.h"
#include "stdio.h" 
#include "common.h"

#include "shell.h"
#include "board.h"

#include "ads7843.h"
#include "spi.h"
#include "types.h"
struct spi_bus bus; 



int main(void)
{
    uint32_t i;
    DelayInit();
    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("HelloWorld\r\n");
    shell_init();
    
    /* 设置Flexbus 速度 速度太快ili9320LCD读点有问题 */
    #ifndef MK10D5
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(4);
    #endif
    
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}


