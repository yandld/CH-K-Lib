#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：I2C地址扫描
     实验平台：凤凰开发板
     板载芯片：MK64FN1MVLQ12
 实验效果：使用I2C通信查询凤凰开发板上的I2C设备，该平台上用于2个i2c设备，
          通过串口将设备的地址发送出去
*/

/* i2c bus scan */
static void I2C_Scan(uint32_t instance)
{
    uint8_t i;
    uint8_t ret;
    for(i = 1; i < 127; i++)
    {
        ret = I2C_BurstWrite(instance , i, 0, 0, NULL, 0);
        if(!ret)
        {
            printf("ADDR:0x%2X(7BIT) | 0x%2X(8BIT) found!\r\n", i, i<<1);
        }
    }
}

int main(void)
{
    uint32_t instance;
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("i2c bus test\r\n");
    /* init i2c */
    instance = I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    I2C_Scan(instance);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


