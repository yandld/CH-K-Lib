#include "shell.h"
#include "clock.h"
#include "spi.h"
#include "gpio.h"
#include "board.h"
#include "spi_abstraction.h"
#include "w25qxx.h"
#include "sram.h"

static uint8_t W25QXX_READ_ID_TABLE[] = {0x90, 0x00, 0x00, 0x00, 0xFF, 0xFF};
static uint8_t test_buffer[8];
void SPI_ISR(void)
{
    static uint8_t i = 1;
    static uint16_t temp = 0;
    if(i == 7)
    {
        SPI_ITDMAConfig(BOARD_SPI_INSTANCE, kSPI_IT_TCF_Disable);
        shell_printf("ID:0x%X\r\n", temp);
        return ;
    }
    if(i == 6)
    {
        temp <<= 8;
    }
    temp |= SPI_ReadWriteByte(BOARD_SPI_INSTANCE,HW_CTAR0, W25QXX_READ_ID_TABLE[i], 1, kSPI_PCS_KeepAsserted);
    i++;
}

#define SPI_FLASH_TEST_LEN  (1024*512)
uint8_t* buf_test = SRAM_START_ADDRESS;

int CMD_SPI(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("SPI TEST CMD\r\n");
    spi_bus bus;
    spi_device device1;
    spi_bus_init(&bus, BOARD_SPI_INSTANCE);
    //设置 CTAR0 1 通道  0给TP 1给SPIFLASH
    bus.bus_config(&bus, HW_CTAR0, kspi_cpol0_cpha0, 30*1000);
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    
    uint8_t buf[6];
    if((argc == 2) && (!strcmp(argv[1], "TP")))
    {
        uint16_t num;
        uint8_t code = 0x90;
        while(1)
        {
            device1.csn = BOARD_TP_SPI_PCSN;
            device1.bus_chl = HW_CTAR0;
            device1.cs_state = kspi_cs_keep_asserted;
            bus.write(&bus, &device1, &code, 1, false);
            DelayUs(6);
            bus.read(&bus, &device1, buf, 2, true);
            num = ((buf[0]<<8) + buf[1])>>4;
            printf("%d\r", num);
        }
        return 0;
    }
    //初始化SPI
   // SPI_QuickInit(BOARD_SPI_MAP, kSPI_CPOL0_CPHA1, 1*1000);
    //安装回调函数
   // SPI_CallbackInstall(BOARD_SPI_INSTANCE, SPI_ISR);
    //开启SPI中断 
   // SPI_ITDMAConfig(BOARD_SPI_INSTANCE, kSPI_IT_TCF);
    w25qxx_device w25qxx1;
    w25qxx1.bus = &bus;
    if(w25qxx_init(&w25qxx1, BOARD_FLASH_SPI_PCSN, HW_CTAR1, 2*1000*1000))
    {
        printf("w25qxx init failed\r\n");
        return 1;
    }
    if(w25qxx1.probe(&w25qxx1))
    {
        printf("no w25qxx deive found\r\n");
        return 1;
    }
    printf("%s,id:0x%X,size:%dKB\r\n",w25qxx1.name, w25qxx1.id, w25qxx1.size/1024);
    for(i=0;i<SPI_FLASH_TEST_LEN;i++)
    {
        buf_test[i] = i;
    }
    if(w25qxx1.write(&w25qxx1, 0, buf_test, SPI_FLASH_TEST_LEN))
    {
        printf("w25qxx write failed\r\n");
        return 1;
    }
    memset(buf_test,0, SPI_FLASH_TEST_LEN);
    if(w25qxx1.read(&w25qxx1, 0, buf_test, SPI_FLASH_TEST_LEN))
    {
        printf("w25qxx read failed\r\n");
        return 1;
    }
    for(i=0;i<SPI_FLASH_TEST_LEN;i++)
    {
        if(buf_test[i] != i%256)
        {
           printf("error:[%d]:%d\r\n", i, buf_test[i]);
        }
    }
    
    return 0;
}

const cmd_tbl_t CommandFun_SPI = 
{
    .name = "SPI",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_SPI,
    .usage = "SPI TEST",
    .complete = NULL,
    .help = "\r\n"
};
