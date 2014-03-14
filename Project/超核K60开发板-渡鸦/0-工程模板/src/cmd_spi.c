#include "shell.h"
#include "clock.h"
#include "spi.h"
#include "gpio.h"
#include "board.h"

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
    temp |= SPI_ReadWriteByte(BOARD_SPI_INSTANCE, W25QXX_READ_ID_TABLE[i], 1, kSPI_PCS_KeepAsserted);
    i++;
}

int CMD_SPI(int argc, char * const * argv)
{
    uint32_t i;
    uint16_t temp = 0;
    shell_printf("SPI TEST CMD\r\n");
    //初始化SPI
    SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14);
    //安装回调函数
    SPI_CallbackInstall(BOARD_SPI_INSTANCE, SPI_ISR);
    //开启SPI中断 
    SPI_ITDMAConfig(BOARD_SPI_INSTANCE, kSPI_IT_TCF);
    //Init CS
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2);
    // start transfer
    SPI_ReadWriteByte(BOARD_SPI_INSTANCE, W25QXX_READ_ID_TABLE[0], 1, kSPI_PCS_KeepAsserted);

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
