#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "dma.h"
#include "pit.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "flash.h"
#include "bootloader.h"

static uint32_t send(uint8_t *buf, uint32_t len)
{
    int i;
    for(i=0; i<len; i++)
    {
        UART_PutChar(HW_UART0, *buf++);
    }
    return len;
}



static uint32_t flash_erase(uint32_t addr)
{
    if(FLASH_EraseSector(addr) == FLASH_OK)
    {
        return BL_FLASH_OK;
    }
    else
    {
        return BL_FLASH_ERR;
    }
}

static uint32_t flash_write(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    if(FLASH_WriteSector(addr, buf, len) == FLASH_OK)
    {
        return BL_FLASH_OK;
    }
    else
    {
        return BL_FLASH_ERR;
    }
}

static Boot_t Boot;

int main(void)
{
    DelayInit();

    
    UART_Init(UART0_RX_PA01_TX_PA02, 115200);
    UART_SetIntMode(HW_UART0, kUART_IntRx, true);
    FLASH_Init();
    
    Boot.AppStartAddr = 0x5000;
    Boot.TimeOut = 2000;
    Boot.FlashPageSize = FLASH_GetSectorSize();
    Boot.send = send;
    Boot.flash_erase = flash_erase;
    Boot.flash_write = flash_write;
    
    BootloaderInit(&Boot);
    
    while(1)
    {
        BootloaderProc();
    }
}



void UART0_IRQHandler(void)
{
    uint8_t ch;
    if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART_C2_RIE_MASK))
    {
        ch = UART0->D;
        GetData(ch);
    }
    /**
        this is very important because in real applications
        OR usually occers, the RDRF will block when OR is asserted 
    */
    if(UART0->S1 & UART_S1_OR_MASK)
    {
        ch = UART0->D;
    }
}
