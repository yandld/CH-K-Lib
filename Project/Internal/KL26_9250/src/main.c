#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "mpu9250.h"
#include "protocol.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"

#include <math.h>

#define DEFAULT_MPU_HZ  (100)
#define  Pitch_error  0
#define  Roll_error   0
#define  Yaw_error    0


//void mpu9250_test(void)
//{
//    uint8_t err;
//    int16_t ax,ay,az,gx,gy,gz,mx,my,mz;
//    while(1)
//    {
//        err = 0;
//    
//        err += mpu9250_read_accel_raw(&ax, &ay, &az);
//        err += mpu9250_read_gyro_raw(&gx, &gy, &gz);
//        err += mpu9250_read_mag_raw(&mx, &my, &mz);
//    
//        if(err)
//        {
//            printf("!err:%d\r\n", err);
//            while(1);
//        }

//        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%d my:%d mz:%d    \r", ax ,ay, az, gx, gy, gz, mx, my, mz);  
//		GPIO_ToggleBit(HW_GPIOC, 3);
//        DelayMs(10);
//    }
//}


static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};

static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}


static  unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;

    return scalar;
}

 /*×Ô¼ìº¯Êý*/
static void run_self_test(void)
{
    int result;

    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) 
    {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
     //   dmp_set_accel_bias(accel);
		printf("setting bias succesfully ......\n");
    }
	else
	{
		printf("bias has not been modified ......\n");
	}
    
}

#define q30  1073741824.0f
static int _quat2angle(long *quat, float *angle)
{
    static float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
    q0 = quat[0] / q30;
    q1 = quat[1] / q30;
    q2 = quat[2] / q30;
    q3 = quat[3] / q30;
            
    angle[0]  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3 + Pitch_error; // pitch
    angle[1]  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3 + Roll_error; // roll
    angle[2]  = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3 + Yaw_error;
    return 0;
}

static int _transmit_data(uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    
    p = buf;
    while(len--)
    {
        UART_WriteByte(HW_UART0, *p++);
    }
    return 0;
}

void dmp_test(void)
{
    unsigned long sensor_timestamp;
    short gyro[3], accel[3], mag[3], sensors;
    static unsigned char more;
    long quat[4];
    float angle[3];
    transmit_user_data send_data;
    uint32_t len;
    static uint8_t buf[64];
    int ret;
    
    ret = mpu_init(NULL);
    printf("mpu9250 init %d\r\n", ret);
    
    ret = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL|INV_XYZ_COMPASS);
    printf("mpu9250 mpu_set_sensor%d\r\n", ret);
    
    ret = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL|INV_XYZ_COMPASS);
    printf("mpu9250 mpu_configure_fifo%d\r\n", ret);
    
    ret = mpu_set_sample_rate(DEFAULT_MPU_HZ);
    printf("mpu9250 mpu_set_sample_rate%d\r\n", ret);
    
    ret = dmp_load_motion_driver_firmware();
    printf("mpu9250 dmp_load_motion_driver_firmware%d\r\n", ret);
    
    ret = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
    printf("mpu9250 dmp_set_orientation%d\r\n", ret);
    
    ret = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT  |
            DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
            DMP_FEATURE_GYRO_CAL);
    printf("mpu9250 dmp_enable_feature%d\r\n", ret);
    
    ret = dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    printf("mpu9250 dmp_set_fifo_rate%d\r\n", ret);
    
    run_self_test();
    
    ret = mpu_set_dmp_state(1);
    
    printf("dmp init complete\r\n");
    
    while(1)
    {
        dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);	
        if (sensors & INV_WXYZ_QUAT )
        {
            _quat2angle(quat, angle);

            mpu_get_compass_reg(mag, NULL);
            
            send_data.trans_gyro[0] = gyro[0];
            send_data.trans_gyro[1] = gyro[1];
            send_data.trans_gyro[2] = gyro[2];
            
            send_data.trans_accel[0] = accel[0];
            send_data.trans_accel[1] = accel[1];
            send_data.trans_accel[2] = accel[2];
            
            send_data.trans_mag[0] = mag[0];
            send_data.trans_mag[1] = mag[1];
            send_data.trans_mag[2] = mag[2];
            
            send_data.trans_pitch = (int16_t)(angle[0]*100);
            send_data.trans_roll = (int16_t)(angle[1]*100);
            send_data.trans_yaw = (int16_t)(angle[2]*10);
            
            /* set buffer */
            len = user_data2buffer(&send_data, buf);
            
            _transmit_data(buf, len);

            GPIO_ToggleBit(HW_GPIOC, 3);
     
        }
    }
}

extern void UART_RX_ISR(uint16_t data);

int main(void)
{
	uint32_t clock;
//    struct mpu_config config;
    
	DelayInit();   
    DelayMs(1);
    
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
    
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);

    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
//    /* sensor init */
//    mpu9250_init(0);
//    
//    config.afs = AFS_16G;
//    config.gfs = GFS_2000DPS;
//    config.mfs = MFS_16BITS;
//    config.aenable_self_test = false;
//    config.genable_self_test = false;
//    mpu9250_config(&config);

    
    dmp_test();
    //mpu9250_test();
    
    while(1)
    {
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(10);
    }
}


void UART_RX_ISR(uint16_t data)
{
//    uint8_t ret;
//    mpu_set_dmp_state(0);
//    
//    ret = mpu_init(NULL);
//    printf("mpu9250 init %d\r\n", ret);
//    
//    ret = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL|INV_XYZ_COMPASS);
//    printf("mpu9250 mpu_set_sensor%d\r\n", ret);
//    
//    ret = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL|INV_XYZ_COMPASS);
//    printf("mpu9250 mpu_configure_fifo%d\r\n", ret);
//    
//    ret = mpu_set_sample_rate(DEFAULT_MPU_HZ);
//    printf("mpu9250 mpu_set_sample_rate%d\r\n", ret);
//    
//    ret = dmp_load_motion_driver_firmware();
//    printf("mpu9250 dmp_load_motion_driver_firmware%d\r\n", ret);
//    
//    ret = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
//    printf("mpu9250 dmp_set_orientation%d\r\n", ret);
//    
//    ret = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT  |
//            DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
//            DMP_FEATURE_GYRO_CAL);
//    printf("mpu9250 dmp_enable_feature%d\r\n", ret);
//    
//    ret = dmp_set_fifo_rate(DEFAULT_MPU_HZ);
//    printf("mpu9250 dmp_set_fifo_rate%d\r\n", ret);
//    
//    run_self_test();
//    
//    
//    mpu_set_dmp_state(1);
//    printf("data!\r\n");
}
