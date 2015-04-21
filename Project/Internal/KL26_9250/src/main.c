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
	uint32_t instance, clock;
	int16_t i = 0;
    struct mpu_config config;
    
	DelayInit();    
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);    
    printf("HelloWorld\r\n");
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("kBusClock:%dHz\r\n", clock);

    /* driver init */
    I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 7, 0);
    DelayMs(10);
    
    /* sensor init */
    mpu9250_init(0);
    
    config.afs = AFS_4G;
    config.gfs = GFS_250DPS;
    config.mfs = MFS_14BITS;
    
    config.enable_aself_test = false;

    
    mpu9250_config(&config);
        
    while(1);
    
	CHZT02_Init();


	//scopeInit();

    while(1)
    {
//		scopeDrawLine(i++,0,0);
		CHZT02_Running();
		testPitch = ch9250_imu_float_euler_angle_t.imu_pitch;
		testRoll = ch9250_imu_float_euler_angle_t.imu_roll;
		testYaw = ch9250_imu_float_euler_angle_t.imu_yaw;
		scopeDrawLine((int16_t)testPitch,(int16_t)testRoll,(int16_t)testYaw);
//		printf("\f");
//		printf("Pitch = %f\r\n",testPitch);
//		printf("Roll = %f\r\n",testRoll);
//		printf("Yaw = %f\r\n",testYaw);
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(50);

		
    }
}


