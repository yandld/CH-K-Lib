#include "shell.h"
#include "common.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "systick.h"
#include "bmp180.h"
#include "gpio.h"
static mpu6050_device mpu6050_device1;
static hmc5883_device hmc_device;
static bmp180_device bmp180_device1;

#define IMU_TEST_I2C_MAP I2C0_SCL_PE19_SDA_PE18

int DoMPU(int argc, char *const argv[])
{
    int16_t ax,ay,az, gx,gy,gz, mx,my,mz;
    int32_t data;
    static uint8_t init;
    if(!init)
    {
        mpu6050_init(&mpu6050_device1, IMU_TEST_I2C_MAP, "mpu6050", 96000);
        hmc5883_init(&hmc_device, IMU_TEST_I2C_MAP, "hmc5883", 96000);
        bmp180_init(&bmp180_device1, IMU_TEST_I2C_MAP, "bmp180", 96000);
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

    mpu6050_device1.read_accel(&mpu6050_device1, &ax, &ay, &az);
    mpu6050_device1.read_gyro(&mpu6050_device1, &gx, &gy, &gz);
    hmc_device.read_data (&hmc_device, &mx, &my, &mz);
    //bmp180 conv will task serveral ms
    bmp180_device1.start_conversion(&bmp180_device1, BMP180_T_MEASURE);
    shell_printf("A:%05d, %05d, %05d G: %05d %05d %05d M:%05d %05d %05d\r\n", ax, ay, az, gx, gz, gz, mx, my, mz);
    if(bmp180_device1.read_temperature(&bmp180_device1, &data))
    {
        shell_printf("bmp180 busy!\r\n");
    }
    else
    {
        shell_printf("bmp180_temptureate:%d\r\n", data);
    }
    bmp180_device1.start_conversion(&bmp180_device1, BMP180_P3_MEASURE);
    DelayMs(25);
    if(bmp180_device1.read_pressure(&bmp180_device1, &data))
    {
        shell_printf("bmp180 busy!\r\n");
    }
    else
    {
        shell_printf("bmp180_presuure:%d\r\n", data);
    }
    return 0;
}

const cmd_tbl_t CommandFun_IMUHW = 
{
    .name = "HWTEST",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoMPU,
    .usage = "HardWare Test for IMU",
    .complete = NULL,
    .help = "HardWare Test for IMU"
};



