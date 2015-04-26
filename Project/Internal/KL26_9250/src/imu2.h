#ifndef _IMU2_H_
#define _IMU2_H_

#include <stdint.h>

#define PI  (3.14159265358979323846f)

extern float pitch, yaw, roll;
extern float q[4];

void imu2_init(void);
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);



#endif
