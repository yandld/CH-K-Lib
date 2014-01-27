//包含超核库所需的头文件
#include "clock.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "stdio.h"
#include "common.h"

//包含 姿态模块所需的头文件
#include "mpu6050/mpu6050.h"
#include "hmc5883/hmc5883.h"
#include "bmp180/bmp180.h"
#include "imu/imu.h"
#include "trans.h"

#define I2C_MAP_URANUS     I2C0_SCL_PE19_SDA_PE18
#define I2C_SPEED_URANUS   (376*1000)

//实习协议传输组件的回调 并连接回调
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
static bmp180_device bmp180_device1;
//实现姿态解算的回调并连接回调
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


uint8_t InitSensor(void)
{
    static int init = 0;
    uint8_t ret = 0;
    if(!init)
    {
        ret += mpu6050_init(&mpu6050_device1, I2C_MAP_URANUS, "mpu6050", I2C_SPEED_URANUS);
        ret += hmc5883_init(&hmc_device, I2C_MAP_URANUS, "hmc5883", I2C_SPEED_URANUS);
        ret += bmp180_init(&bmp180_device1, HW_I2C0, 0x77, I2C_SPEED_URANUS);
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
    //int32_t temperature;
    //int32_t pressure;
    imu_float_euler_angle_t angle;
    imu_raw_data_t raw_data;
    DelayInit(); //Init Delay
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
        //获取欧拉角 获取原始角度
        imu_get_euler_angle(&angle, &raw_data);
        //printf("%05d %05d %05d\r", (int16_t)angle.imu_pitch, (int16_t)angle.imu_roll, (int16_t)angle.imu_yaw);
        //将数据送给发送构建
        send_data.trans_accel[0] = raw_data.ax;
        send_data.trans_accel[1] = raw_data.ay;
        send_data.trans_accel[2] = raw_data.az;
        send_data.trans_gyro[0] = raw_data.gx;
        send_data.trans_gyro[1] = raw_data.gy;
        send_data.trans_gyro[2] = raw_data.gz;
        send_data.trans_mag[0] = raw_data.mx;
        send_data.trans_mag[1] = raw_data.my;
        send_data.trans_mag[2] = raw_data.mz;     
        send_data.trans_pitch = (int16_t)angle.imu_pitch*100;
        send_data.trans_roll = (int16_t)angle.imu_roll*100;
        send_data.trans_yaw = (int16_t)angle.imu_yaw*10;
        //压力采集程序 由于上位机不传送压力，所以先注释掉
        /*
        while(1)
        {
            bmp180_start_conversion(&bmp180_device1, BMP180_T_MEASURE);
            DelayMs(50);
            bmp180_read_temperature(&bmp180_device1, &temperature);
            bmp180_start_conversion(&bmp180_device1, BMP180_P3_MEASURE);
            DelayMs(50);
            bmp180_read_pressure(&bmp180_device1, &pressure);
            printf("T:%05d P:%05d\r\n", temperature, pressure);
        }
        */
        //发送数据
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


#ifdef USE_FULL_ASSERT
void assert_failed(char * file, uint32_t line)
{
	//断言失败检测
	while(1);
}
#endif
