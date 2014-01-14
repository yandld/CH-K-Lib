
#ifndef _IMU_H_
#define _IMU_H_

#include <math.h>
#include <stdint.h>

//!< return 0 if you get data succ. reutrn else if data get failed
typedef struct
{
    uint32_t (*imu_get_accel)(int16_t * ax, int16_t * ay, int16_t * az);
    uint32_t (*imu_get_gyro)(int16_t * gx, int16_t * gy, int16_t * gz);
    uint32_t (*imu_get_mag)(int16_t * mx, int16_t * my, int16_t * mz);
}imu_io_install_t;

typedef struct 
{
    float imu_pitch;
    float imu_roll;
    float imu_yaw;
}imu_float_euler_angle_t;


//!< API functions
uint32_t imu_io_install(imu_io_install_t * IOInstallStruct);
uint32_t imu_get_euler_angle(imu_float_euler_angle_t * angle);

#endif
