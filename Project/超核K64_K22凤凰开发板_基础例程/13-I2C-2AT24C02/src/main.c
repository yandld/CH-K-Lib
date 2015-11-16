#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "at24cxx.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/*
     实验名称：I2C设备AT24C02
     实验平台：凤凰开发板
     板载芯片：MK64FN1MVLQ12
 实验效果：使用I2C通信与开发板上的AT24C02存储芯片通信，进行测试，成功后串口发送数据
*/
int main(void)
{
    uint32_t ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("at24c02 test\r\n");
    
    I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    

    ret = eep_init(0);
    if(ret)
    {
        printf("at24cxx not found\r\n");
        while(1);
    }

    if(eep_self_test(0, 256))
    {
        printf("at24cxx self test failed\r\n");
        return 1;  
    }
    else
    {
        printf("at24cxx test ok!\r\n");
    }
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


