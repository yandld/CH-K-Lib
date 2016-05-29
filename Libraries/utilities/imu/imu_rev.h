
#ifndef _IMU_REV_H_
#define _IMU_REV_H_

#include <stdint.h>


enum imu_rev_mode
{
    IMU_REV_Polling,
    IMU_REV_Interrupt,
};

typedef struct 
{
    int16_t accl[3];
    int16_t gyro[3];
    int16_t mag[3];
    int16_t yaw;
    int16_t pitch;
    int16_t roll;
    int32_t presure;
}imu_data;




typedef struct imu_rev_init* imu_rev_init_t;

//!< API functions
void imu_rev_init(void);
void imu_rev_process(char ch);
void imu_get_data(imu_data *imu);


#endif

