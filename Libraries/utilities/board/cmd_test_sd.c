#include "shell.h"
#include "gpio.h"
#include "board.h"
#include "sd.h"

__align(4) static uint8_t sd_buffer[512];

int DoSD(int argc, char * const argv[])
{
    uint32_t i;
    printf("SD test\r\n");
    printf("please insert SD card...\r\n");
    SD_QuickInit(10000000);
    printf("SD size:%dMB\r\n", SD_GetSizeInMB());
    SD_ReadSingleBlock(0, sd_buffer);
    printf("sectoer 0 data:\r\n");
    for(i = 0; i < 512; i++)
    {
        printf("0x%02X ", sd_buffer[i]);
    }

    return 0;
}

SHELL_EXPORT_CMD(DoSD, sd , sd card test)
    
