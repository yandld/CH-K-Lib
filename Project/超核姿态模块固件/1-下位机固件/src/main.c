#include <string.h>

#include "chlib_k.h"
#include "board.h"
#include "protocol.h"

#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "imu.h"
#include "trans.h"

#define FLASH_DATA_ADDR  (0x8000 - 1024)

struct calibration_data
{
    int  magic;
    float mxg;       /* mag x gain */
    float myg;
    float mzg;
    int   mxo;
    int   myo;
    int   mzo;       /* mag z offset */
    
    int axo;
    int ayo;
    int azo; 
    
    int gxo;
    int gyo;
    int gzo; 
};

static void _print_cal_data(struct calibration_data * cal)
{
    printf("cal data:\r\n");
    printf("axo:%04d ayo:%04d azo:%04d\r\n", cal->axo, cal->ayo, cal->azo);
    printf("gxo:%04d gyo:%04d gzo:%04d\r\n", cal->gxo, cal->gyo, cal->gzo);
    printf("mxo:%04d myo:%04d mzo:%04d\r\n", cal->mxo, cal->myo, cal->mzo);
    printf("mxg:%04f myg:%04f mzg:%0fd\r\n", cal->mxg, cal->myg, cal->mzg);
    
}

void MagnetometerCalibration(struct calibration_data * cal)
{
    uint32_t i;
    int r;
    int16_t x,y,z;
    int16_t xmax, xmin, ymax, ymin, zmax, zmin;
    xmax = 0;
    xmin = 0xFFFF;
    ymax = 0;
    ymin = 0xFFFF;
    zmax = 0;
    zmin = 0xFFFF;
    printf("start calibration...\r\n");

    for(i=0;i<1000;i++)
    {
        r = hmc5883_read_data(&x, &y, &z);
        if(!r)
        {
            if(xmax < x) xmax = x;
            if(xmin > x) xmin = x;
            if(ymax < y) ymax = y;
            if(ymin > y) ymin = y;
            if(zmax < z) zmax = z;
            if(zmin > z) zmin = z; 
        }
        DelayMs(10);
        printf("time:%04d xmax:%04d xmin:%04d ymax:%04d ymin%04d zmax:%04d zmin:%04d\r", i,xmax,xmin,ymax,ymin,zmax,zmin);
    }
    cal->mxo = (xmax + xmin) / 2;
    cal->mxg=1;
    cal->myo = (ymax + ymin) / 2;
    cal->myg= (float)(ymax - ymin) / (float)(xmax -xmin);
    cal->mzo = (zmax + zmin) / 2;
    cal->mzg= (float)(zmax - zmin) / (float)(xmax -xmin);
    /* see if we get data correct */
    if((cal->myg < 0.8) || (cal->mzg < 0.8))
    {
        printf("cal failed, setting to default param\r\n");
        /* inject with default data */
        cal->mxo = 0;
        cal->myo = 0;
        cal->mzo = 0;
        cal->mxg = 1;
        cal->myg = 1;
        cal->mzg = 1;
    }
    printf("g X:%f Y:%f Z:%f\r\n", cal->mxg, cal->myg, cal->mzg);
    printf("o X:%d Y:%d Z:%d\r\n", cal->mxo, cal->myo, cal->mzo);
    cal->magic = 0x5ACB;
}


static struct calibration_data cal_data;
static imu_float_euler_angle_t angle;
static imu_raw_data_t raw_data;
static int32_t temperature;
static int32_t pressure;

int hmc5883_read_data2(int16_t* x, int16_t* y, int16_t* z)
{
    int r;
    int16_t xraw,yraw,zraw;
    r = hmc5883_read_data(&xraw, &yraw, &zraw);
    if(!r)
    {
        *x = cal_data.mxg  *(xraw - cal_data.mxo);
        *y = cal_data.myg *(yraw - cal_data.myo);
        *z = cal_data.mzg *(zraw - cal_data.mzo);
    }
    return r;
}

static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = mpu6050_read_accel,
    .imu_get_gyro = mpu6050_read_gyro,
    .imu_get_mag = hmc5883_read_data2,
};
 

static void send_data_process(void)
{
    static uint8_t buf[64];
    uint32_t len;
    static transmit_user_data data;
    data.trans_accel[0] = raw_data.ax;
    data.trans_accel[1] = raw_data.ay;
    data.trans_accel[2] = raw_data.az;
    data.trans_gyro[0] = raw_data.gx;
    data.trans_gyro[1] = raw_data.gy;
    data.trans_gyro[2] = raw_data.gz;
    data.trans_mag[0] = raw_data.mx;
    data.trans_mag[1] = raw_data.my;
    data.trans_mag[2] = raw_data.mz;
    data.trans_pitch = (int16_t)(angle.imu_pitch*100);
    data.trans_roll = (int16_t)(angle.imu_roll*100);
    data.trans_yaw = 1800 + (int16_t)(angle.imu_yaw*10);
    data.trans_pressure = pressure;
    
    /* set buffer */
    len = user_data2buffer(&data, buf);
    trans_start_send_data(buf, len);

}

void mpu6050_test(void)
{
    mpu6050_init(0);
    struct mpu_config config;
    
    config.afs = AFS_16G;
    config.gfs = GFS_2000DPS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu6050_config(&config);
    
    uint8_t err;
    int16_t ax,ay,az,gx,gy,gz,mx,my,mz;
    
    static int16_t mmx,mmy,mmz;
    while(1)
    {
        err = 0;
    
        err += mpu6050_read_accel(&ax, &ay, &az);
        err += mpu6050_read_gyro(&gx, &gy, &gz);
        err += hmc5883_read_data(&mx, &my, &mz);

        mmx = 0.9*mmx + 0.1*mx;
        mmy = 0.9*mmy + 0.1*my;
        mmz = 0.9*mmz + 0.1*mz;
        
        if(err)
        {
            printf("!err:%d\r\n", err);
            while(1);
        }

        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d   \r", ax ,ay, az, gx, gy, gz, mx, my, mz);  
        DelayMs(5);
    }
}

int init_sensor(void)
{
    uint32_t ret;

    I2C_QuickInit(I2C0_SCL_PE19_SDA_PE18, 100*1000);
    
    DelayMs(1);
    ret = mpu6050_init(0);
    printf("mpu6050 failed:%d\r\n", ret);
    
    ret = hmc5883_init(0);
    printf("hmc5883 failed:%d\r\n", ret);
    
    ret = bmp180_init(0);
    printf("bmp180 failed:%d\r\n", ret);
    
    return ret;
}

#define BMP_STATUS_T_START          (0x00)
#define BMP_STATUS_T_COMPLETE       (0x01)
#define BMP_STATUS_P_START          (0x02)
#define BMP_STATUS_P_COMPLETE       (0x03)
#define BMP_STATUS_T_WAIT           (0x04)
#define BMP_STATUS_P_WAIT           (0x05)

int main(void)
{
    int i;
    uint32_t sector_size;
    uint32_t led_flag;
    static int bmpStatus = BMP_STATUS_T_START;
    uint32_t ret;
    uint32_t uart_instance;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    
    uart_instance = UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("PS!\r\n");
    
    GPIO_QuickInit(HW_GPIOE, 18, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 19, kGPIO_Mode_OPP);
    
    init_sensor();

   // sector_size = FLASH_GetSectorSize();
    memcpy(&cal_data, (void*)FLASH_DATA_ADDR, sizeof(cal_data));
    if(cal_data.magic ==  0x5ACB)
    {
        printf("read cal data from flash succ\r\n");
        _print_cal_data(&cal_data);
    }
    else
    {
        printf("read cal data from flash err!\r\n");
        MagnetometerCalibration(&cal_data);
        FLASH_EraseSector(FLASH_DATA_ADDR);
        FLASH_WriteSector(FLASH_DATA_ADDR, (const uint8_t*)&cal_data, sizeof(cal_data));
    }
    
    trans_init(HW_DMA_CH1, uart_instance);
    imu_io_install(&IMU_IOInstallStruct1);
    
    PIT_QuickInit(HW_PIT_CH2, 1000*1000);
    
    static uint32_t time, load_val, fac_us;
    CLOCK_GetClockFrequency(kBusClock, &fac_us);
    fac_us /= 1000000;
    
   // mpu6050_test();
    
    while(1)
    {
        time = PIT_GetCounterValue(HW_PIT_CH2);
        PIT_ResetCounter(HW_PIT_CH2);
        load_val = PIT_GetCounterValue(HW_PIT_CH2);
        time = load_val - time;
        time /= fac_us;

        ret = imu_get_euler_angle(&angle, &raw_data);

        halfT = ((float)time)/1000/2000;
       // printf("time:%f   \r", halfT);

        switch(bmpStatus)
        {
            case BMP_STATUS_T_START:
                bmp180_start_conversion(BMP180_T_MEASURE);
                bmpStatus = BMP_STATUS_T_WAIT;
                break;
            case BMP_STATUS_T_WAIT:
                if(!is_conversion_busy())
                {
                    bmpStatus = BMP_STATUS_T_COMPLETE;
                }
                break;
            case BMP_STATUS_T_COMPLETE:
                bmp180_read_temperature(&temperature);
                bmpStatus = BMP_STATUS_P_START;
                break;
            case BMP_STATUS_P_START:
                bmp180_start_conversion(BMP180_P3_MEASURE);
                bmpStatus = BMP_STATUS_P_WAIT;
                break;
            case BMP_STATUS_P_WAIT:
                if(!is_conversion_busy())
                {
                    bmpStatus = BMP_STATUS_P_COMPLETE;
                }
                break;
            case BMP_STATUS_P_COMPLETE:
                bmp180_read_pressure(&pressure);
                bmpStatus = BMP_STATUS_T_START;
                break;
            default:
                break;
        }
        
        send_data_process();
        led_flag++; led_flag %= 10;
        if(!led_flag)
        {
            GPIO_ToggleBit(HW_GPIOA, 1);
        }
        
    }
}


