#include <math.h>

#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "adxl345.h"

int main(void)
{
    float fdata[3];
    float angle[3];
    short x,y,z;
    short ax, ay, az;
    uint32_t ret;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    
    printf("adxl345 test\r\n");
    
    ret = adxl345_init(0);
    if(ret)
    {
        printf("no device found!%d\r\n", ret);
    }

    adxl345_calibration();
    
    while(1)
    {
        if(!adxl345_readXYZ(&x, &y, &z))
        {
            fdata[0] = (float)x;
            fdata[1] = (float)y;
            fdata[2] = (float)z;
            angle[0] = atan2f(-fdata[0],fdata[2])*57.3;
            angle[1] = atan2f(-fdata[1],fdata[2])*57.3;
            
            printf("X:%3d Y:%3d Z:%3d AngleX:%0.4fC AngleY:%0.4fC  \r", x, y, z, angle[0], angle[1]);
            
        }
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(20);
    }
}


