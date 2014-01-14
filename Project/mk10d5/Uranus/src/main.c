#include "sys.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "clock.h"
#include "cpuidy.h"

#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "imu.h"
#include "trans.h"

static void Putc(uint8_t data)
{
    UART_WriteByte(HW_UART1, data);
}

static uint8_t Getc(void)
{
    uint8_t ch;
    while(UART_ReadByte(HW_UART1, &ch));
    return ch;
}

trans_io_install_t TRANS_IOInstallStruct1 = 
{
	.trans_getc = Getc,
	.trans_putc = Putc,
};

static mpu6050_device mpu6050_device1;
static hmc5883_device hmc_device;
//static bmp180_device bmp180_device1;

static uint32_t imu_get_mag(int16_t * mx, int16_t * my, int16_t *mz)
{
    hmc_device.read_data(&hmc_device, mx, my, mz);
    return 0;
}

static uint32_t imu_get_accel(int16_t * ax, int16_t * ay, int16_t *az)
{
    mpu6050_device1.read_accel(&mpu6050_device1, ax,ay,az);
    return 0;  
}

static uint32_t imu_get_gyro(int16_t * gx, int16_t * gy, int16_t * gz)
{
    mpu6050_device1.read_gyro(&mpu6050_device1, gx,gy,gz);
    return 0;
}

static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = imu_get_accel,
    .imu_get_gyro = imu_get_gyro,
    .imu_get_mag = imu_get_mag,
};


#define IMU_TEST_I2C_MAP I2C0_SCL_PE19_SDA_PE18

uint8_t InitSensor(void)
{
    static int init = 0;
    uint8_t ret = 0;
    if(!init)
    {
        ret += mpu6050_init(&mpu6050_device1, IMU_TEST_I2C_MAP, "mpu6050", 96000);
        ret += hmc5883_init(&hmc_device, IMU_TEST_I2C_MAP, "hmc5883", 96000);
     //   ret += bmp180_init(&bmp180_device1, IMU_TEST_I2C_MAP, "bmp180", 96000);
        init = 1;
    }
    if(ret)
    {
        return ret;
    }
    return 0;
}


int main(void)
{
    uint32_t i;
    uint8_t ret;
    uint32_t counter = 0;
    imu_float_euler_angle_t angle;
    //SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
    SYSTICK_DelayInit(); //Init Delay
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);  //Init UART and printf
    GPIO_QuickInit(HW_GPIOA, 1 , kGPIO_Mode_OPP);   //Init LED
    //Blink LED to indicate system is running
    for(i = 0; i < 10; i++)
    {
        GPIO_ToggleBit(HW_GPIOA, 1);
        DelayMs(50);
    }
    //init sensor
    ret = InitSensor();
    if(ret)
    {
        printf("Sensor init failed! code:%d\r\n", ret);
    }
    // install imu interface function
    imu_io_install(&IMU_IOInstallStruct1);
    // install trans uart for send data
    trans_io_install(&TRANS_IOInstallStruct1);
    
    trans_user_data_t send_data;
    while(1)
    {
        imu_get_euler_angle(&angle);
        printf("%05d %05d %05d\r", (int16_t)angle.imu_pitch, (int16_t)angle.imu_roll, (int16_t)angle.imu_yaw);
        
        send_data.trans_pitch = (int16_t)angle.imu_pitch*100;
        send_data.trans_roll = (int16_t)angle.imu_roll*100;
        send_data.trans_yaw = (int16_t)angle.imu_yaw*10;
        trans_send_pactket(send_data);
        //blink the LED
        counter++;
        counter %= 10; 
        if(!counter)
        {
            GPIO_ToggleBit(HW_GPIOA, 1);
        }
    }

}

void assert_failed(char * file, uint32_t line)
{
	//¶ÏÑÔÊ§°Ü¼ì²â
	//shell_printf("assert_failed:line:%d %s\r\n",line,file);
	while(1);
}

