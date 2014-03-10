#include "shell.h"
#include "clock.h"
#include "spi.h"
#include "gpio.h"
#include "board.h"


int CMD_SPI(int argc, char * const * argv)
{
    uint32_t i;
    uint16_t temp = 0;
    shell_printf("SPI TEST CMD\r\n");
    SPI_InitTypeDef SPI_InitStruct1;
    SPI_InitStruct1.baudrateDivSelect = kSPI_BaudrateDiv_128;
    SPI_InitStruct1.CPHA = kSPI_CPHA_1Edge;
    SPI_InitStruct1.CPOL = kSPI_CPOL_InactiveLow;
    SPI_InitStruct1.dataSizeInBit = 8;
    SPI_InitStruct1.instance = BOARD_SPI_INSTANCE;
    SPI_InitStruct1.mode = kSPI_Master;
    
    SPI_Init(&SPI_InitStruct1);
    
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2);
    
    SPI_ReadWriteByte(BOARD_SPI_INSTANCE, 0x90, 1, kSPI_PCS_KeepAsserted);
    SPI_ReadWriteByte(BOARD_SPI_INSTANCE, 0x00, 1, kSPI_PCS_KeepAsserted);
    SPI_ReadWriteByte(BOARD_SPI_INSTANCE, 0x00, 1, kSPI_PCS_KeepAsserted);
    SPI_ReadWriteByte(BOARD_SPI_INSTANCE, 0x00, 1, kSPI_PCS_KeepAsserted);
    temp |= SPI_ReadWriteByte(BOARD_SPI_INSTANCE, 0xFF, 1, kSPI_PCS_KeepAsserted)<<8;
    temp |= SPI_ReadWriteByte(BOARD_SPI_INSTANCE, 0xFF, 1, kSPI_PCS_ReturnInactive);
    shell_printf("ID:0x%X\r\n", temp);

    
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
