#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"
#include "i2c.h"
#include "at24cxx.h"

int cmd_i2c(int argc, char * const argv[])
{
    int ret;
    I2C_QuickInit(I2C1_SCL_PC01_SDA_PC02, 100*1000);
    at24cxx_init(HW_I2C1);
    ret = at24cxx_self_test();
    return ret;
}

int main(void)
{
    int ret;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    printf("5_EEPORM demo\r\n");
    ret = cmd_i2c(0, NULL);
    
    if(ret)
    {
        printf("EEPOEM test failed\r\n");
    }
    else
    {
        printf("EEPORM test ok!\r\n");
    }
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(500);
    }
}

