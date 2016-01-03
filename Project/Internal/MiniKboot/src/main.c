#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "systick.h"

#include "bl_core.h"
#include "bl_cfg.h"

static bool jump = false;

uint8_t bl_hw_if_read_byte(void)
{
    uint16_t ch;
    while(UART_ReadByte(0, &ch))
    {
        if(jump == true)
        {
            application_run();
        }
    }
    return (ch & 0xFF);
}

void bl_hw_if_write(const uint8_t *buffer, uint32_t length)
{
    while(length--)
    {
        putchar(*buffer++);
    }
}

/* 
blhost.exe -p COM52 ,115200 -- get-property 1
*/

int main(void)
{
    DelayInit();
    DelayMs(10);
    SYSTICK_Init(100*1000);
    SYSTICK_ITConfig(true);
    SYSTICK_Cmd(true);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    while(1)
    {
        bootloader_run();
    }
}

void SysTick_Handler(void)
{
    static int timeout;
    
    if(timeout > 2)
    {
        SYSTICK_ITConfig(false);
        if((bootloader_isActive() == false) && (IsAppAddrValidate() == true))
        {
            jump = true;
        }
    }
    timeout++;
}

