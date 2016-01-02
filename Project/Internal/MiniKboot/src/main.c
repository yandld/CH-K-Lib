#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "systick.h"

#include "bl_core.h"
#include "bl_cfg.h"


uint8_t bl_hw_if_read_byte(void)
{
    return getchar();
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

 bool IsAppAddrOK(void)
{
    uint32_t *vectorTable = (uint32_t*)APPLICATION_BASE;
    uint32_t pc = vectorTable[1];
    
    if (pc < APPLICATION_BASE || pc > TARGET_FLASH_SIZE)
    {
        return false;
    }
    else
    {
        return true;
    } 
}

int main(void)
{
    int i, ch;
    uint32_t instance; /*´æ·Å UART µÄÄ£¿éºÅ */
    DelayInit();
    DelayMs(10);
    SYSTICK_Init(5000*1000);
    SYSTICK_ITConfig(true);
    SYSTICK_Cmd(true);
    instance = UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
  //  bl_hw_init();
    
    while(1)
    {
        
        bootloader_run();
    }

}

void SysTick_Handler(void)
{
    static int timeout;
    if((bootloader_isActive() == false) && (IsAppAddrOK() == true) && (timeout > 2))
    {
        application_run();
    }
    if(timeout > 2)
    {
        SYSTICK_ITConfig(false);
    }
    timeout++;
}

