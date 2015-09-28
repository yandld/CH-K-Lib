#include "common.h"
#include "uart.h"
#include "dma.h"
#include "flash.h"

#include "def.h"
#include "bootloader.h"


int main(void)
{
    DelayInit();
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    BootloaderInit(UART0, 115200, 2000);
    
    while(1)
    {
        BootloaderProc(); //执行Bootloader主进程
    }
}
