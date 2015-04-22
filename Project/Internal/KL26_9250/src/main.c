#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "scope.h"
#include "i2c.h"
#include "CHZT02.h"
#include "imu.h"
#include "mpu9250.h"

double testRoll,testPitch,testYaw;
extern imu_io_install_t  			ch9250_imu_io_install_t;
extern imu_float_euler_angle_t		ch9250_imu_float_euler_angle_t;   
extern imu_raw_data_t				ch9250_imu_raw_data_t;	



void mpu9250_test(void)
{
    uint8_t err;
    int16_t ax,ay,az,gx,gy,gz,mx,my,mz;
    
    err = 0;
    
    err += mpu9250_read_accel_raw(&ax, &ay, &az);
    err += mpu9250_read_gyro_raw(&gx, &gy, &gz);
    err += mpu9250_read_mag_raw(&mx, &my, &mz);
    
    if(err)
    {
        printf("!err:%d\r\n", err);
        while(1);
    }
    printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d    \r", ax ,ay, az, gx, gy, gz, mx, my, mz);
}

int main(void)
{
	uint32_t instance, clock;
    struct mpu_config config;
    
	DelayInit();    
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);    
    printf("HelloWorld\r\n");
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("kBusClock:%dHz\r\n", clock);

    /* MCU driver init */
    I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 7, 0);
    DelayMs(10);
    
    /* sensor init */
    mpu9250_init(0);
    
    config.afs = AFS_16G;
    config.gfs = GFS_250DPS;
    
    config.enable_aself_test = false;

    mpu9250_config(&config);
        

    while(1)
    {
        mpu9250_test();
        
////		scopeDrawLine(i++,0,0);
//		CHZT02_Running();
//		testPitch = ch9250_imu_float_euler_angle_t.imu_pitch;
//		testRoll = ch9250_imu_float_euler_angle_t.imu_roll;
//		testYaw = ch9250_imu_float_euler_angle_t.imu_yaw;
//		scopeDrawLine((int16_t)testPitch,(int16_t)testRoll,(int16_t)testYaw);
//		printf("\f");
//		printf("Pitch = %f\r\n",testPitch);
//		printf("Roll = %f\r\n",testRoll);
//		printf("Yaw = %f\r\n",testYaw);
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(10);

		
    }
}


