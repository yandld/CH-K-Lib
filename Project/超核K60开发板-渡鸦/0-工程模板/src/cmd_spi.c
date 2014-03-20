#include "shell.h"
#include "clock.h"
#include "spi.h"
#include "gpio.h"
#include "board.h"
#include "spi_abstraction.h"


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

int CMD_SPI(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("SPI TEST CMD\r\n");
    spi_bus bus;
    spi_device device1;
    spi_bus_init(&bus, kspi_cpol0_cpha1, BOARD_SPI_INSTANCE, 30*1000);
    
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
    SPI_CallbackInstall(BOARD_SPI_INSTANCE, SPI_ISR);
    //开启SPI中断 
   // SPI_ITDMAConfig(BOARD_SPI_INSTANCE, kSPI_IT_TCF);
    device1.csn = BOARD_FLASH_SPI_PCSN;
    device1.cs_state = kspi_cs_keep_asserted;
    bus.write(&bus, &device1, W25QXX_READ_ID_TABLE, 4, false);
    bus.read(&bus, &device1, buf, 2, true);

    printf("buf[0]:%x\r\n", buf[0]);
    printf("buf[1]:%x\r\n", buf[1]);
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
