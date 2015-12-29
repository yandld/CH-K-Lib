#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "bl_core.h"

uint8_t bl_hw_if_read_byte(void)
{
    return getchar();
}


void bl_hw_if_write(const uint8_t *buffer, uint32_t length)
{
    while(length--)
    {
        UART_WriteByte(0, *buffer++);
    }
}

/* 
blhost.exe -p COM52 ,115200 -- get-property 1
*/

int main(void)
{
    uint32_t instance; /*存放 UART 的模块号 */
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);

    instance = UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);


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

