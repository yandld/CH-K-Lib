#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "flexbus.h"

#include "sram.h"

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("Flexbus SRAM test\r\n");
    
    /* ³õÊ¼»¯ SRAM */
    SRAM_Init();
    /* SRAM ×Ô²â */
    if(!SRAM_SelfTest())
    {
        printf("sram test ok!\r\n");
    }
    else
    {
        printf("sram test failed!\r\n");
    }
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


