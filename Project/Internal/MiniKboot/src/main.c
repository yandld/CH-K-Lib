#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "bl_core.h"

void bl_hw_if_write(const uint8_t *buf, uint32_t len)
{
    while(len--)
    {
        UART_WriteByte(0, *buf++);
    }
}

static void UART_RX_ISR(uint16_t byteReceived)
{
    bootloader_data_sink(byteReceived);
}

int main(void)
{
    uint32_t instance; /*存放 UART 的模块号 */
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);

    instance = UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);

  //  bl_hw_init();
    
  //  if (stay_in_bootloader())
    {
        bootloader_run();
    }
 //   else
    {
  //      application_run();
    }
    
    // Should never reach here.
    return 0;

    while(1)
    {
        UART_WriteByte(instance, 'h');
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}

