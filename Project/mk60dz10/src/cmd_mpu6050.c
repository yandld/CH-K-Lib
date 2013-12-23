#include "shell.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "gpio.h"
static mpu6050_device mpu6050_device1;
static hmc5883_device hmc_device;
static bmp180_device bmp180_device1;

#define MPU6050_I2C_MAP I2C1_SCL_PC10_SDA_PC11

int DoMPU(int argc, char *const argv[])
{
    int16_t ax,ay,az, gx,gy,gz, mx,my,mz;
    uint16_t i;
    static uint8_t init;
    if(!init)
    {
        mpu6050_init(&mpu6050_device1, MPU6050_I2C_MAP, "mpu6050", 96000);
        hmc5883_init(&hmc_device, MPU6050_I2C_MAP, "hmc5883", 96000);
        bmp180_init(&bmp180_device1, MPU6050_I2C_MAP, "bmp180", 96000);
        init = 1;
    }
    if(mpu6050_device1.probe(&mpu6050_device1))
    {
        shell_printf("mpu6050 detected fail\r\n");
        return 0;
    }
    if(hmc_device.probe(&hmc_device))
    {
        shell_printf("hmc5883 detected fail\r\n");
        return 0;
    }
    if(bmp180_device1.probe(&bmp180_device1))
    {
        shell_printf("bmp180 detected fail\r\n");
        return 0;
    }
    shell_printf("mmpu6050 succ.\r\n");
    shell_printf("hum5883 succ.\r\n");
    shell_printf("bmp180 succ.\r\n");
    for(i=0;i<400;i++)
    {
        mpu6050_device1.read_accel(&mpu6050_device1, &ax, &ay, &az);
        mpu6050_device1.read_gyro(&mpu6050_device1, &gx, &gy, &gz);
        hmc_device.read_data (&hmc_device, &mx, &my, &mz);
        shell_printf("A:%05d, %05d, %05d G: %05d %05d %05d M:%05d %05d %05d\r", ax, ay, az, gx, gz, gz, mx, my, mz);
        DelayMs(50);
    }
    shell_printf("data display finish\r\n");

    return 0;
}

const cmd_tbl_t CommandFun_MPU6050 = 
{
    .name = "MPU",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoMPU,
    .usage = "MPU test",
    .complete = NULL,
    .help = "\r\n"
    "I2C scan - scan I2C bus\r\n"
    "I2C it   - I2C interrupt test"
};



