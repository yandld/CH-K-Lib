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

extern int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance);
static int DO_SPI_FLASH(int argc, char * const argv[])
{
    uint32_t i;
    uint16_t x,y;
    #define SPI_FLASH_TEST_LEN  (1024*512)
    volatile uint8_t* buf_test = SRAM_START_ADDRESS;
    uint32_t ret;
    static struct spi_bus bus;
    ret = kinetis_spi_bus_init(&bus, HW_SPI2);
    ret = ads7843_init(&bus, 0);
    ret = w25qxx_init(&bus, 1);
    
    
    for(i=0;i<100;i++)
    {
        ads7843_readX(&x);
        ads7843_readY(&y);
        printf("X:%04d Y:%04d\r", x, y);
        DelayMs(10);
    }
    /* probe w25qxx */
    if(w25qxx_probe())
    {
        printf("no w25qxx deived found\r\n");
        return 1;
    }
    printf("%s detected!\r\n", w25qxx_get_name());
    /*
    for(i=0;i<SPI_FLASH_TEST_LEN;i++)
    {
        buf_test[i] = i;
    }
    if(w25qxx_write(0, (uint8_t*)buf_test, SPI_FLASH_TEST_LEN))
    {
        printf("w25qxx write failed\r\n");
        return 1;
    }
    memset((uint8_t*)buf_test,0, SPI_FLASH_TEST_LEN);
    if(w25qxx_read(0, (uint8_t*)buf_test, SPI_FLASH_TEST_LEN))
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
    */
    printf("spi flash test finish!\r\n");
    return 0;
}



int CMD_SPI(int argc, char *const argv[])
{
    shell_printf("SPI TEST CMD\r\n");
    
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); //
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); //
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); //    

    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    if((argc == 2) && (!strcmp(argv[1], "FLASH")))
    {
        return DO_SPI_FLASH(argc, argv);
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
