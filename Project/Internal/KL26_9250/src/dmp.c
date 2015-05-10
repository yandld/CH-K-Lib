#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "dmp.h"


#define DEFAULT_MPU_HZ      (100)
#define q30  1073741824.0f

#ifndef NULL
#define NULL    0
#endif



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

void dmp_init(void)
{
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
}


int quat2angle(long *quat, float *angle)
{
    static float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
    q0 = quat[0] / q30;
    q1 = quat[1] / q30;
    q2 = quat[2] / q30;
    q3 = quat[3] / q30;
            
    angle[1]  = -asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
    angle[0]  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
    angle[2]  = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
    
    return 0;
}
