#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"
#include "FlashOS.H"
#include "flash.h"

/* chip's Flash size and sector size can be found in RM */
#define SECTOR_SIZE         (0x000400)
#define DEV_SIZE            (0x100000)


static void FlashTest(void)
{
    int addr, i,err,secNo;
    uint8_t *p;
    static uint8_t buf[SECTOR_SIZE];
      
    for(i=0;i<SECTOR_SIZE;i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for (secNo = 20; secNo < (DEV_SIZE/SECTOR_SIZE); secNo++)
    {
        addr = secNo*SECTOR_SIZE;
        printf("program addr:0x%X ", addr);
        err = 0;
        err += FLASH_EraseSector(addr);
        err += FLASH_WriteSector(addr, buf, SECTOR_SIZE);
        if(err)
        {
            printf("issue command failed %d\r\n", err);
            while(1);
            return;
        }
        p = (uint8_t*)(addr);
        
        for(i=0;i<SECTOR_SIZE;i++)
        {
            if(*p++ != (i%0xFF))
            {
                err++;
                printf("[%d]:0x%02X ", i, *p);
            }
        }
        if(!err)
        {
            printf("verify OK\r\n");
        }
        else
        {
            printf("verify ERR\r\n");
        }
    }
}

int main(void)
{
    uint32_t i;
    uint32_t ticks;
    uint32_t r;
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    PIT_QuickInit(HW_PIT_CH0, 1000*1000);
    
    FlashTest();
    
    printf("flash test completed!\r\n");
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


