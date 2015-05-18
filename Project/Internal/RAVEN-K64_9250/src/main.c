#include <string.h>

#include "chlib_k.h"

#include "protocol.h"
#include "chlib_k.h"
#include "mpu9250.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "virtual_eep.h"
#include "imu.h"
#include "trans.h"
#include "filter.h"


#define FLASH_DATA_ADDR  (0xA000 - 1024)

float IMULoopTime;
static struct calibration_data cal_data;
static imu_float_euler_angle_t angle;

static int32_t temperature;
static int32_t pressure;

struct calibration_data
{
    int  magic;
    float mg[3];        /* mag x gain */
    int16_t mo[3];      /* mag z offset */ 
    int16_t ao[3];
    int16_t go[3];
};

static void _print_cal_data(struct calibration_data * cal)
{
    
    printf("cal data:\r\n");
    
    printf("gyro offset:%d %d %d \r\n", cal->go[0], cal->go[1], cal->go[2]);
    printf("acce offset:%d %d %d \r\n", cal->ao[0], cal->ao[1], cal->ao[2]);
    printf("magn offset:%d %d %d \r\n", cal->mo[0], cal->mo[1], cal->mo[2]);
    printf("mag gain:%f %f %f \r\n",    cal->mg[0], cal->mg[1], cal->mg[2]);
}

void MagnetometerCalibration(struct calibration_data * cal)
{
    uint32_t i;
    int r;
    int16_t mdata[3];
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
        r = mpu9250_read_mag_raw(mdata);
        if(!r)
        {
            if(xmax < mdata[0]) xmax = mdata[0];
            if(xmin > mdata[0]) xmin = mdata[0];
            if(ymax < mdata[1]) ymax = mdata[1];
            if(ymin > mdata[1]) ymin = mdata[1];
            if(zmax < mdata[2]) zmax = mdata[2];
            if(zmin > mdata[2]) zmin = mdata[2]; 
        }
        DelayMs(10);
        printf("time:%04d xmax:%04d xmin:%04d ymax:%04d ymin%04d zmax:%04d zmin:%04d\r", i,xmax,xmin,ymax,ymin,zmax,zmin);
    }
    cal->mo[0] = (xmax + xmin) / 2;
    cal->mg[0]=1;
    cal->mo[1] = (ymax + ymin) / 2;
    cal->mg[1]= (float)(ymax - ymin) / (float)(xmax -xmin);
    cal->mo[2] = (zmax + zmin) / 2;
    cal->mg[2]= (float)(zmax - zmin) / (float)(xmax -xmin);
    /* see if we get data correct */
    if((cal->mg[1] < 0.8) || (cal->mg[2] < 0.8))
    {
        printf("cal failed, setting to default param\r\n");
        /* inject with default data */
        cal->mo[0] = 0;
        cal->mo[1] = 0;
        cal->mo[2] = 0;
        cal->mg[0] = 1;
        cal->mg[1] = 1;
        cal->mg[2] = 1;
        cal->magic = 0x00;
        return;
    }
    else
    {
        cal->magic = 0x5ACB;
    }
}



static void send_data_process(imu_float_euler_angle_t *angle, int16_t *adata, int16_t *gdata, int16_t *mdata)
{
    int i;
    static uint8_t buf[64];
    uint32_t len;
    static transmit_user_data data;
    
    for(i=0;i<3;i++)
    {
        data.trans_accel[i] = adata[i];
        data.trans_gyro[i] = gdata[i];
        data.trans_mag[i] = mdata[i];
    }
    data.trans_pitch = (int16_t)(angle->P*100);
    data.trans_roll = (int16_t)(angle->R*100);
    data.trans_yaw = 1800 + (int16_t)(angle->Y*10);
    data.trans_pressure = pressure;
    
    /* set buffer */
    len = ano_encode(&data, buf);
    uart_dma_send(buf, len);

}

void mpu9250_test(void)
{
    mpu9250_init(0);
    struct mpu_config config;
    
    config.afs = AFS_8G;
    config.gfs = GFS_1000DPS;
    config.mfs = MFS_14BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);
    
    uint8_t err;
    int16_t mdata[3], gdata[3], adata[3];
    while(1)
    {
        err = 0;
    
        err += mpu9250_read_accel_raw(adata);
        err += mpu9250_read_gyro_raw(gdata);
        err += mpu9250_read_mag_raw(mdata);
    
        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d    \r", adata[0] ,adata[1], adata[2], gdata[0], gdata[1], gdata[2], mdata[0], mdata[1], mdata[2]);  
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(5);
    }
}

int init_sensor(void)
{
    uint32_t ret;

    I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
    
    DelayMs(50);
    ret = mpu9250_init(0);
    printf("mpu9250 init:%d\r\n", ret);
    
    struct mpu_config config;
    
    config.afs = AFS_8G;
    config.gfs = GFS_1000DPS;
    config.mfs = MFS_16BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);
    
    return ret;
}

static void ShowInfo(void)
{
    uint32_t clock;
    printf("%s\r\n", "URANUS2 V1.00");
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%dHz\r\n", clock);
    
}

#define BMP_STATUS_T_START          (0x00)
#define BMP_STATUS_T_COMPLETE       (0x01)
#define BMP_STATUS_P_START          (0x02)
#define BMP_STATUS_P_COMPLETE       (0x03)
#define BMP_STATUS_T_WAIT           (0x04)
#define BMP_STATUS_P_WAIT           (0x05)

int main(void)
{
    int i, led_flag;

    static int bmpStatus = BMP_STATUS_T_START;
    uint32_t ret;
    uint32_t uart_instance;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    uart_instance = UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    ShowInfo();
    veep_init();
   // GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
   // GPIO_QuickInit(HW_GPIOE, 1, kGPIO_Mode_OPP);
    
    init_sensor();

    veep_read((uint8_t*)&cal_data, sizeof(cal_data));

    if(cal_data.magic ==  0x5ACB)
    {
        printf("read cal data from flash succ\r\n");
        _print_cal_data(&cal_data);
    }
    else
    {
        printf("read cal data from flash err!\r\n");
        MagnetometerCalibration(&cal_data);
        veep_write((uint8_t*)&cal_data, sizeof(cal_data));
    }
    
    
    PIT_QuickInit(HW_PIT_CH2, 1000*1000);
    
    static uint32_t time, load_val, fac_us;
    CLOCK_GetClockFrequency(kBusClock, &fac_us);
    fac_us /= 1000000;
   
   // mpu9250_test();
 
    uart_dma_init(HW_DMA_CH1, uart_instance);
    
    int16_t adata[3], gdata[3], mdata[3];
    static float fadata[3], fgdata[3], fmdata[3];
    while(1)
    {
        mpu9250_read_accel_raw(adata);
        mpu9250_read_gyro_raw(gdata);
        mpu9250_read_mag_raw(mdata);
        
        for(i=0;i<3;i++)
        {
            mdata[i] = cal_data.mg[i]*(mdata[i] - cal_data.mo[i]);
        }
    
        time = PIT_GetCounterValue(HW_PIT_CH2);
        PIT_ResetCounter(HW_PIT_CH2);
        load_val = PIT_GetCounterValue(HW_PIT_CH2);
        time = load_val - time;
        time /= fac_us;

        float factor[3];
        factor[0] = lpf_1st_factor_cal(IMULoopTime, 92);
        factor[1] = lpf_1st_factor_cal(IMULoopTime, 92);
        factor[2] = lpf_1st_factor_cal(IMULoopTime, 5);
        for(i=0;i<3;i++)
        {
            fadata[i] = lpf_1st(fadata[i], (float)adata[i], factor[0]);
            fgdata[i] = lpf_1st(fgdata[i], (float)gdata[i], factor[1]);
            fmdata[i] = lpf_1st(fmdata[i], (float)mdata[i], factor[2]);
        }

        ret = imu_get_euler_angle(fadata, fgdata, fmdata, &angle);
        
        halfT = ((float)time)/1000/2000;
        IMULoopTime = halfT*2;
//        switch(bmpStatus)
//        {
//            case BMP_STATUS_T_START:
//                bmp180_start_conversion(BMP180_T_MEASURE);
//                bmpStatus = BMP_STATUS_T_WAIT;
//                break;
//            case BMP_STATUS_T_WAIT:
//                if(!is_conversion_busy())
//                {
//                    bmpStatus = BMP_STATUS_T_COMPLETE;
//                }
//                break;
//            case BMP_STATUS_T_COMPLETE:
//                bmp180_read_temperature(&temperature);
//                bmpStatus = BMP_STATUS_P_START;
//                break;
//            case BMP_STATUS_P_START:
//                bmp180_start_conversion(BMP180_P3_MEASURE);
//                bmpStatus = BMP_STATUS_P_WAIT;
//                break;
//            case BMP_STATUS_P_WAIT:
//                if(!is_conversion_busy())
//                {
//                    bmpStatus = BMP_STATUS_P_COMPLETE;
//                }
//                break;
//            case BMP_STATUS_P_COMPLETE:
//                bmp180_read_pressure(&pressure);
//                bmpStatus = BMP_STATUS_T_START;
//                break;
//            default:
//                break;
//        }
        
        send_data_process(&angle, adata, gdata, mdata);
        
        led_flag++; led_flag %= 10;
        if(!led_flag)
        {
            GPIO_ToggleBit(HW_GPIOA, 1);
        }
        
    }
}


