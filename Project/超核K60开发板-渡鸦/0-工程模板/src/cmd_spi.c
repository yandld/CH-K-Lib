#include "shell.h"
#include "spi.h"
#include "gpio.h"
#include "board.h"
#include "spi_abstraction.h"
#include "w25qxx.h"
#include "sram.h"
#include "ads7843.h"

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
volatile uint8_t* buf_test = SRAM_START_ADDRESS;
//uint8_t buf_test[SPI_FLASH_TEST_LEN];


static int DO_SPI_FLASH(int argc, char * const argv[])
{
    uint32_t i;
    struct spi_bus bus; 
    spi_bus_init(&bus, BOARD_SPI_INSTANCE);
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    w25qxx_device w25qxx1;
    w25qxx1.bus = &bus;
    if(w25qxx_init(&w25qxx1, BOARD_FLASH_SPI_PCSN, HW_CTAR1, 64*1000*1000))
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
  //  printf("erasing chip ...\r\n");
  //  w25qxx1.erase_chip(&w25qxx1);
  //  printf("erasing complete\r\n");
    if(w25qxx1.write(&w25qxx1, 0, (uint8_t*)buf_test, SPI_FLASH_TEST_LEN))
    {
        printf("w25qxx write failed\r\n");
        return 1;
    }
    memset((uint8_t*)buf_test,0, SPI_FLASH_TEST_LEN);
    if(w25qxx1.read(&w25qxx1, 0, (uint8_t*)buf_test, SPI_FLASH_TEST_LEN))
    {
        printf("w25qxx read failed\r\n");
        return 1;
    }
    for(i=0;i<SPI_FLASH_TEST_LEN;i++)
    {
        if(buf_test[i] != i%256)
        {
           printf("error:[0x%X]:%d\r\n", i, buf_test[i]);
        }
    }
    return 0;
}


static int DO_SPI_TP(int argc, char * const argv[])
{
    uint16_t x,y;
    struct spi_bus bus; 
    spi_bus_init(&bus, BOARD_SPI_INSTANCE);
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    struct ads7843_device ads7843;
    ads7843.bus = &bus;
    ads7843_init(&ads7843, BOARD_TP_SPI_PCSN, HW_CTAR0, 20*1000);
    ads7843.probe(&ads7843);
    while(1)
    {
        ads7843.readX(&ads7843, &x);
        ads7843.readY(&ads7843, &y);
        printf("X:%04d Y:%04d\r", x, y);
    }
}

int CMD_SPI(int argc, char *const argv[])
{
    shell_printf("SPI TEST CMD\r\n");
    //设置 CTAR0 1 通道  0给TP 1给SPIFLASH
    if((argc == 2) && (!strcmp(argv[1], "FLASH")))
    {
        return DO_SPI_FLASH(argc, argv);
    }
    if((argc == 2) && (!strcmp(argv[1], "TP")))
    {
        return DO_SPI_TP(argc, argv);
    }
    //初始化SPI
   // SPI_QuickInit(BOARD_SPI_MAP, kSPI_CPOL0_CPHA1, 1*1000);
    //安装回调函数
   // SPI_CallbackInstall(BOARD_SPI_INSTANCE, SPI_ISR);
    //开启SPI中断 
   // SPI_ITDMAConfig(BOARD_SPI_INSTANCE, kSPI_IT_TCF);
    return CMD_RET_USAGE;
}

const cmd_tbl_t CommandFun_SPI = 
{
    .name = "SPI",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_SPI,
    .usage = "SPI <CMD> (CMD = TP,FLASH)",
    .complete = NULL,
    .help = "SPI <CMD> (CMD = TP,FLASH)"
};
