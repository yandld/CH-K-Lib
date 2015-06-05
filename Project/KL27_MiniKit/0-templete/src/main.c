#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "dma.h"
#include "lpuart.h"
#include "i2c.h"

#include "at24cxx.h"
#include "w25qxx.h"

#define LED0_PORT   HW_GPIOC
#define LED0_PIN    3
#define LED1_PORT   HW_GPIOA
#define LED1_PIN    1
#define LED2_PORT   HW_GPIOE
#define LED2_PIN    0
#define LED3_PORT   HW_GPIOA
#define LED3_PIN    2
    
void at24cxx_test(void)
{
    int ret;
    I2C_QuickInit(I2C1_SCL_PC01_SDA_PC02, 100*1000);
    at24cxx_init(1);
    ret = at24cxx_self_test();
    (ret)?
    (printf("at24cxx test failed!\r\n")):
    (printf("at24cxx test ok!\r\n"));
}


static uint8_t SPI_xfer(uint8_t data, uint8_t cs)
{
    uint8_t val;
    GPIO_WriteBit(HW_GPIOC, 4, 0);
    val = SPI_ReadWriteByte(HW_SPI0, (uint8_t)data);
    GPIO_WriteBit(HW_GPIOC, 4, cs);
    return val;
}

static int w25qxx_test(void)
{
    SPI_QuickInit(SPI0_SCK_PC05_MOSI_PC06_MISO_PC07, kSPI_CPOL1_CPHA1, 5*1000*1000);
    GPIO_QuickInit(HW_GPIOC, 4, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOC, 4, 1);
    
    uint32_t i, block, buf_size,j;
    static uint8_t buf[4*1024];
    struct w25qxx_attr_t w25qxx;
    
    w25qxx_init(0, SPI_xfer);
    w25qxx_get_attr(&w25qxx);
    buf_size = sizeof(buf);
    block = w25qxx.size/buf_size;
    
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

void dma_test(void)
{
    int i, err;
    uint8_t src_buf[16];
    uint8_t dest_buf[16];

    for(i=0;i<sizeof(src_buf);i++)
    {
        src_buf[i] = i;
        dest_buf[i] = 0;
    }
    
    DMA_InitTypeDef Init;

    Init.chl = HW_DMA_CH0;
    Init.chlTriggerSource = MUX0_DMAREQ;
    Init.triggerSourceMode = kDMA_TriggerSource_Normal;
    Init.transferByteCnt = sizeof(src_buf);

    Init.sAddr = (uint32_t)src_buf;
    Init.sAddrIsInc = true;
    Init.sDataWidth = kDMA_DataWidthBit_8;
    Init.sMod = kDMA_ModuloDisable;

    Init.dAddr = (uint32_t)dest_buf;
    Init.dAddrIsInc = true;
    Init.dDataWidth = kDMA_DataWidthBit_8;
    Init.dMod = kDMA_ModuloDisable;
    DMA_Init(&Init);
    
    DMA_EnableRequest(HW_DMA_CH0);
    while(DMA_GetTransferByteCnt(HW_DMA_CH0) != 0);

    for(i=0;i<sizeof(src_buf);i++)
    {
        if(dest_buf[i] != src_buf[i])
        {
            err++;
        }
    }
    if(err)
    {
        printf("dma test failed!\r\n");
    }
    else
    {
        printf("dma test ok!\r\n");
    }

}

int main(void)
{
    DelayInit();
    /* LED */
    GPIO_QuickInit(LED0_PORT, LED0_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED1_PORT, LED1_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED2_PORT, LED2_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED3_PORT, LED3_PIN, kGPIO_Mode_OPP);
    /* KEY */
    GPIO_QuickInit(HW_GPIOB, 0, kGPIO_Mode_IPU);
    /* UART */
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);

    printf("HelloWorld!\r\n");

    dma_test();
    at24cxx_test();
    w25qxx_test();
    while(1)
    {
        GPIO_ToggleBit(LED0_PORT, LED0_PIN);
        GPIO_ToggleBit(LED1_PORT, LED1_PIN);
        GPIO_ToggleBit(LED2_PORT, LED2_PIN);
        GPIO_ToggleBit(LED3_PORT, LED3_PIN);

        DelayMs(100);
    }
}


