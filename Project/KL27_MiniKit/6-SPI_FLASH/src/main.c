#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"
#include "spi.h"
#include "w25qxx.h"


static uint32_t xfer(uint8_t *buf_in, uint8_t *buf_out, uint32_t len, uint8_t cs_state)
{
    uint8_t dummy_in;
    
    if(!buf_in)
        buf_in = &dummy_in;
    GPIO_WriteBit(HW_GPIOC, 4, 0);
    while(len--)
    {
        *buf_in = SPI_ReadWriteByte(HW_SPI0, *buf_out); 
        if(buf_out)
            buf_out++;
        if(buf_in != &dummy_in)
            buf_in++;
    }
    GPIO_WriteBit(HW_GPIOC, 4, cs_state);
    return len;
}

static uint32_t _get_reamin(void)
{
    return 0;
}

int cmd_spi(int argc, char * const argv[])
{
    struct w25qxx_init_t init;
    
    init.delayms = DelayMs;
    init.get_reamin = _get_reamin;
    init.xfer = xfer;
    
    SPI_QuickInit(SPI0_SCK_PC05_MOSI_PC06_MISO_PC07, kSPI_CPOL1_CPHA1, 5*1000*1000);
    GPIO_QuickInit(HW_GPIOC, 4, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOC, 4, 1);
    
    
    
    uint32_t i, block, buf_size,j;
    static uint8_t buf[4*1024];
    struct w25qxx_attr_t w25qxx;
    
    w25qxx_init(&init);
    w25qxx_get_attr(&w25qxx);
    buf_size = sizeof(buf);
    block = w25qxx.size/buf_size;
    
    printf("%s found!\r\n", w25qxx.name);
    printf("test unit count:%d\r\n", block);
    
    /* erase chip */
    printf("erase all chips...\r\n");
    w25qxx_erase_chip();
    printf("erase complete\r\n");
    
    for(i=0; i<block; i++)
    {
        printf("verify addr:0x%X(%d)...\r\n", i*buf_size, i);
        for(j=0;j<sizeof(buf);j++)
        {
            buf[j] = j % 0xFF;
        }
        w25qxx_write(i*block, buf, buf_size);
        memset(buf, buf_size, 0);
        w25qxx_read(i*block, buf, buf_size);
        
        /* varify */
        for(j=0;j<sizeof(buf);j++)
        {
            if(buf[j] != (j%0xFF))
            {
                printf("%d error\r\n", j);
            }
        }
    }
    return 0;
}



int main(void)
{
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    printf("6-SPI_FLASH demo\r\n");
    cmd_spi(0, NULL);

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(500);
    }
}

