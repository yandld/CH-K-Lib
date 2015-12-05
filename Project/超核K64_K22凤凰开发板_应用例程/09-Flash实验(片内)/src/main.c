#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"
#include "flash.h"

/*
     实验名称：片内Flash实验
     实验平台；凤凰开发板
     板载芯片：MK64FN1MVLQ12
 实验效果：使用芯片内部的空间存储数据，小灯闪烁。
*/
/* chip's Flash size and sector size can be found in RM */
#define DEV_SIZE            (0x80000)


static void FlashTest(void)
{
    int addr, i, err, secNo, sector_size;
    uint8_t *p;
    static uint8_t buf[4096];
      
    sector_size = FLASH_GetSectorSize();
    printf("flash sector size:%d\r\n", sector_size);
    
    for(i=0;i<sector_size;i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for (secNo = 20; secNo < (DEV_SIZE/sector_size); secNo++)
    {
        addr = secNo*sector_size;
        printf("program addr:0x%X ", addr);
        err = 0;
        err += FLASH_EraseSector(addr);
        err += FLASH_WriteSector(addr, buf, sector_size);
        if(err)
        {
            printf("issue command failed %d\r\n", err);
            while(1);
        }
        p = (uint8_t*)(addr);
        
        for(i=0;i<sector_size;i++)
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
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("flash demo\r\n");
    
    FlashTest();
    
    printf("flash test completed!\r\n");
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


