#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"

#ifdef __CC_ARM /* Keil */
int test(void) __attribute__((section(".ARM.__at_0x8000")));
int test(void) 
#elif __ICCARM__ /* IAR */
int test(void) @".myfun"
#endif
{
    printf("I am test function\r\n");
    return 0;
}

#ifdef __CC_ARM
const char var __attribute__((section(".ARM.__at_0x8100")));
#elif __ICCARM__
__no_init char var@0x20000000;
#endif
int main(void)
{

    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("DMA UART transmit test\r\n");
    printf("fun addr:0x%08X\r\n", (uint32_t)test);
    printf("var addr:0x%08X\r\n", (uint32_t)&var);
    
    void(*theFunc)(void);
    theFunc = (void(*)(void))(0x8000-1);
    theFunc();
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(200);
    }
}


