#include "gpio.h"
#include "uart.h"
#include "sd.h"

uint8_t sd_buffer[512];

int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("SD test\r\n");
    printf("please insert SD card...\r\n");
    SD_QuickInit(20000000);
    printf("SD size:%dMB\r\n", SD_GetSizeInMB());
    /* 读取0扇区数据 */
    SD_ReadSingleBlock(0, sd_buffer);
    /* 打印0扇区数据 */
    printf("sectoer 0 data:\r\n");
    for(i = 0; i < 512; i++)
    {
        printf("0x%02X ", sd_buffer[i]);
    }
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


