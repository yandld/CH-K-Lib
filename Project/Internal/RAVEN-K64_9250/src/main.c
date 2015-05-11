#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "mpu9250.h"


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

void mpu9250_test(void)
{
    mpu9250_init(0);
    struct mpu_config config;
    
    config.afs = AFS_16G;
    config.gfs = GFS_2000DPS;
    config.mfs = MFS_16BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);
    
    uint8_t err;
    int16_t ax,ay,az,gx,gy,gz,mx,my,mz;
    
    static int16_t mmx,mmy,mmz;
    while(1)
    {
        err = 0;
    
        err += mpu9250_read_accel_raw(&ax, &ay, &az);
        err += mpu9250_read_gyro_raw(&gx, &gy, &gz);
        err += mpu9250_read_mag_raw(&mx, &my, &mz);
    
        mmx = 0.9*mmx + 0.1*mx;
        mmy = 0.9*mmy + 0.1*my;
        mmz = 0.9*mmz + 0.1*mz;
        
        float angle;
         angle = atan2(mmy, mmx)*57.3;
        if(err)
        {
            printf("!err:%d\r\n", err);
            while(1);
        }

        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d %f   \r", ax ,ay, az, gx, gy, gz, mx, my, mz, angle);  
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(5);
    }
}

int main(void)
{
    uint32_t instance;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    
    
    printf("i2c bus test\r\n");
    /* init i2c */
    instance = I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    
    mpu9250_test();
    
  //  I2C_Scan(instance);
    
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


