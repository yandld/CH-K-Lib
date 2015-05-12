#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "mpu9250.h"

#include <math.h>

#include "mltypes.h"

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
    
        mmx = 9*mmx + 1*mx;
        mmy = 9*mmy + 1*my;
        mmz = 9*mmz + 1*mz;
        mmx /= 10;
        mmy /= 10;
        mmz /= 10;
        
        if(err)
        {
            printf("!err:%d\r\n", err);
            while(1);
        }

        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d %f   \r", ax ,ay, az, gx, gy, gz, mx, my, mz, 0);  
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(5);
    }
}


int _MLPrintLog (int priority, const char* tag, const char* fmt, ...)
{
    printf("_MLPrintLog\r\n");
}

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

int main(void)
{
    inv_error_t result;

    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    
    
    printf("i2c bus test\r\n");
    /* init i2c */
    I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    
  //  mpu9250_test();
    
  //  I2C_Scan(instance);
    
  result = inv_init_mpl();
  if (result) {
      printf("Could not initialize MPL.\n");
  }
  
    inv_enable_quaternion();
    inv_enable_9x_sensor_fusion();
    inv_enable_fast_nomot();
    /* inv_enable_motion_no_motion(); */
    /* inv_set_no_motion_time(1000); */

    /* Update gyro biases when temperature changes. */
    inv_enable_gyro_tc();
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


