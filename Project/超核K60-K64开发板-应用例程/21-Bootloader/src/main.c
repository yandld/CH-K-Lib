#include <string.h>

#include "common.h"
#include "uart.h"
#include "systick.h"


#include "bl_core.h"
#include "bl_cfg.h"


/*
1. download this program to chip
2. open Uranus.exe, reset chip, then press Connect within 1S.
3. the App need to be linked at 0x5000, you can use 0-template as bootloader project template. swtich project option to flash_bootlaoder.
4. then find the bin/hex file your bootloader project generaterd. then select bin file and download.
5. program will automcically run after download complete
*/

#define BOOTLOAD_TIMEOUT        (300)
static bool jump = false;
    
void bl_deinit_interface(void)
{
    UART_DeInit(HW_UART0);
}

uint8_t bl_hw_if_read_byte(void)
{
    uint16_t ch;
    while(UART_ReadByte(HW_UART0, &ch) != 0)
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


int main(void)
{
    DelayInit();
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    SYSTICK_Init(1000*100);
    SYSTICK_Cmd(true);
    SYSTICK_ITConfig(true);
    while(1)
    {
        bootloader_run();
    }
}


void SysTick_Handler(void)
{
    static int timeout;
    
    if(timeout > (BOOTLOAD_TIMEOUT/100))
    {
        SYSTICK_ITConfig(false);
        if((bootloader_isActive() == false) && (IsAppAddrValidate() == true))
        {
            jump = true;
        }
    }
    timeout++;
}

void NMI_isr(void)
{
    SYSTICK_ITConfig(false);
    jump = false;
}
