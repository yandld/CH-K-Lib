#include "shell.h"
#include "crc.h"
#include "board.h"


static int DoCRC(int argc, char * const argv[])
{
    uint32_t result;
    uint8_t buffer[10] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30};
    printf("CRC 16 test\r\n");
    result = CRC16_GenerateSoftware((uint8_t*)buffer, 10);
    printf("crc16:0x%04x\r\n", result);

    CRC_InitTypeDef CRC_InitStruct;
    CRC_InitStruct.crcWidth = 16;
    CRC_InitStruct.seed = 0x0000U;
    CRC_InitStruct.polynomial = 0x1021U;
    CRC_InitStruct.writeTranspose = kCRCNoTranspose;
    CRC_InitStruct.readTranspose = kCRCNoTranspose;
    CRC_InitStruct.complementRead = false;
    CRC_Init(&CRC_InitStruct);

    result = CRC_Generate(buffer, 10);
    printf("crc16:0x%04x\r\n", result);
    
    return 0;
}

SHELL_EXPORT_CMD(DoCRC, crc , crc test)
    

