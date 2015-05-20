
#ifndef _IMU_H_
#define _IMU_H_

#include <math.h>
#include <stdint.h>


#define SLIDING_FILTER_DEEP     (3)



typedef struct 
{
    float P;
    float R;
    float Y;
}imu_float_euler_angle_t;



//!< API functions
uint32_t imu_get_euler_angle(float *adata, float *gdata, float *mdata, imu_float_euler_angle_t *angle);
extern float halfT;
#endif
