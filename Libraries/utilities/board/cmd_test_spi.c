#include "shell.h"
#include "gpio.h"
#include "board.h"

static  QuickInit_Type SPI_QuickInitTable[] =
{
    { 1, 4, 2, 0, 3, 0}, //SPI1_SCK_PE02_MOSI_PE01_MISO_PE00
    { 0, 4, 2,17, 3, 0}, //SPI0_SCK_PE17_MOSI_PE18_MISO_PE19
    { 0, 0, 2,15, 3, 0}, //SPI0_SCK_PA15_MOSI_PA16_MISO_PA17
    { 0, 2, 2, 5, 3, 0}, //SPI0_SCK_PC05_MOSI_PC06_MISO_PC07
    { 0, 3, 2, 1, 3, 0}, //SPI0_SCK_PD01_MOSI_PD02_MISO_PD03
    { 1, 3, 2, 5, 3, 0}, //SPI1_SCK_PD05_MOSI_PD06_MISO_PD07
};

int DoSPI(int argc, char * const argv[])
{
    int i;
    printf("board:%s fun:%s\r\n",BOARD_NAME, __func__);
    for(i=0;i<ARRAY_SIZE(SPI_QuickInitTable);i++)
    {
        printf("(0X%08XU)\r\n", QuickInitEncode(&SPI_QuickInitTable[i]));
    }
    return 0;
}

SHELL_EXPORT_CMD(DoSPI, spi , spi test)

