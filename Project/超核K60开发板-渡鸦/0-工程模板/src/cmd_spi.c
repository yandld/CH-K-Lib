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
    temp |= SPI_ReadWriteByte(BOARD_SPI_INSTANCE, W25QXX_READ_ID_TABLE[i], 1, kSPI_PCS_KeepAsserted);
    i++;
}

int CMD_SPI(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("SPI TEST CMD\r\n");
    SPI_ABS_Init(kSPI_ABS_CPOL0_CPHA1, 20*1000);
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    uint8_t buf[6];
    if((argc == 2) && (!strcmp(argv[1], "TP")))
    {
        uint16_t num;
        while(1)
        {
        SPI_ABS_ReadWriteByte(0xD0, BOARD_TXP2046_SPI_PCS, kSPI_ABS_CS_KeepAsserted);
        num = SPI_ABS_ReadWriteByte(0xFF, BOARD_TXP2046_SPI_PCS, kSPI_ABS_CS_KeepAsserted);
        num<<=8;
        num += SPI_ABS_ReadWriteByte(0xFF, BOARD_TXP2046_SPI_PCS, kSPI_ABS_CS_ReturnInactive);  
        num>>=4;
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

    // start transfer
    //SPI_ReadWriteByte(BOARD_SPI_INSTANCE, W25QXX_READ_ID_TABLE[0], 1, kSPI_PCS_KeepAsserted); 
    SPI_ABS_xfer(W25QXX_READ_ID_TABLE, buf, 1, kSPI_ABS_CS_KeepAsserted, sizeof(W25QXX_READ_ID_TABLE));
    printf("buf[4]:%x\r\n", buf[4]);
    printf("buf[5]:%x\r\n", buf[5]);
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
