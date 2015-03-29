#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "at24cxx.h"
/* 从此例程开始 使用英文注释 */

static struct i2c_bus bus;
extern int kinetis_i2c_bus_init(struct i2c_bus* bus, uint32_t instance);

int main(void)
{
    uint32_t ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("adxl345 test\r\n");
    /* init i2c */
    ret = kinetis_i2c_bus_init(&bus, HW_I2C0);
    /* pinmux */
    PORT_PinMuxConfig(HW_GPIOB, 2, kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOB, 3, kPinAlt2);
    
    static struct i2c_bus bus;
    ret = kinetis_i2c_bus_init(&bus, HW_I2C0);
    ret = at24cxx_init(&bus, "at24c02");
    if(ret)
    {
        printf("at24cxx not found\r\n");
        while(1);
    }
    printf("at24cxx size:%d btye\r\n", at24cxx_get_size());
    if(at24cxx_self_test())
    {
        printf("at24cxx self test failed\r\n");
        return 1;  
    }
    
    printf("at24cxx test ok!\r\n");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


