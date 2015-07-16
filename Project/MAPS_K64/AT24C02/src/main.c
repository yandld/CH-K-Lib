#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "at24cxx.h"

int cmd_i2c(int argc, char * const argv[])
{
    int ret;
    I2C_QuickInit(I2C0_SCL_PE24_SDA_PE25, 100*1000);
    at24cxx_init(HW_I2C1);
    ret = at24cxx_self_test();
    return ret;
}

int main(void)
{
    int ret;
    DelayInit();
	
    GPIO_QuickInit(HW_GPIOB, 4, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 7, kGPIO_Mode_OPP);

    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    
    printf("EEPORM demo\r\n");
    ret = cmd_i2c(0, NULL);
    
    if(ret)
    {
        printf("EEPOEM test failed\r\n");
    }
    else
    {
        printf("EEPORM test ok!\r\n");
    }
    
    printf("AT24C02 test!\r\n");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOB, 4);
        DelayMs(500);
    }
}


