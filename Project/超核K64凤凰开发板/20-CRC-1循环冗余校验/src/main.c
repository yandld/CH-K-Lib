#include "chlib_k.h"

 //本测试仅仅用于循环冗余校验
int main(void)
{
    uint32_t result;
    uint8_t buffer[10] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30};
    DelayInit();

    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    /* CRC16 CCITT协议 */
    CRC_QuickInit(kCRC16_XMODEM);
    result = CRC16_GenerateSoftware(buffer, 10);
    printf("CRC 16 software:0x%04x\r\n", result);
    result = CRC_Generate(buffer, 10);
    printf("crc16 hardware:0x%04x\r\n", result);
    
    while(1);

}


