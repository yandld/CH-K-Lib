#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "adxl345.h"


static struct i2c_bus bus;
extern int kinetis_i2c_bus_init(struct i2c_bus* bus, uint32_t instance);

int main(void)
{
    short x,y,z;
    short ax, ay, az;
    uint32_t ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    I2C_Scan(I2C0_SCL_PB02_SDA_PB03);
    
    ret = adxl345_init(0);
    if(ret)
    {
        printf("no device found!%d\r\n", ret);
    }
    
    printf("adxl345 test\r\n");
    /* init i2c */
    ret = kinetis_i2c_bus_init(&bus, HW_I2C0);
    /* pinmux */
    //PORT_PinMuxConfig(HW_GPIOB, 2, kPinAlt2);
    //PORT_PinMuxConfig(HW_GPIOB, 3, kPinAlt2);
    
    
    
//    if(ret)
//    {
//        printf("i2c bus init failed!\r\n");
//        return 1;
//    }
//    if(adxl345_init(&bus))
//    {
//        printf("adxl345 init failed\r\n");
//        return 1;
//    }
//    if(adxl345_probe())
//    {
//        printf("no adxl345 device found\r\n");
//        return 1;
//    }
//    printf("device found:ID:0x%02X\r\n", adxl345_get_addr());
//    adxl345_calibration();
    while(1)
    {
        if(!adxl345_readXYZ(&x, &y, &z))
        {
         //   adxl345_convert_angle(x, y, z, &ax, &ay, &az);
            printf("X:%3d Y:%3d Z:%3d AX:%3d AY:%3d AZ:%3d  \r\n", x, y, z, ax, ay ,az); 
        }
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(100);
    }
}


