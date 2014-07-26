#include "shell.h"
#include "uart.h"
#include "common.h"

void UART_ISR(uint16_t data)
{
    printf("%s %c\r\n", __func__, data);
}


static int DoUART(int argc, char * const argv[])
{
    uint32_t clock;
    uint32_t instance;
    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%d\r\n", clock);
    CLOCK_GetClockFrequency(kFlashClock, &clock);
    printf("FlashClock:%d\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("BusClock:%d\r\n", clock);
    UART_CallbackRxInstall(instance, UART_ISR);
    UART_ITDMAConfig(instance, kUART_IT_Rx, true);
}

SHELL_EXPORT_CMD(DoUART, uart , uart test)
    

