#include "common.h"
#include "uart.h"
#include "dma.h"
#include "flash.h"
#include "bootloader.h"


static uint32_t send(uint8_t *buf, uint32_t len)
{
    int i;
    for(i=0; i<len; i++)
    {
        UART_WriteByte(HW_UART0, *buf++);
    }
    return len;
}

static uint32_t receive(uint8_t *buf, uint32_t len)
{
    if(UART_ReadByte(HW_UART0, (uint16_t*)buf) == 0)
    {
        return 1;
    }
    return 0;
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
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 921600);
    FLASH_Init();
    
    Boot.AppStartAddr = 0x5000;
    Boot.TimeOut = 2000;
    Boot.FlashPageSize = FLASH_GetSectorSize();
    Boot.send = send;
    Boot.receive = receive;
    Boot.flash_erase = flash_erase;
    Boot.flash_write = flash_write;
    
    BootloaderInit(&Boot);
    
    while(1)
    {
        BootloaderProc();
    }
}
