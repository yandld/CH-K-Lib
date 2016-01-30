/**
  ******************************************************************************
  * @file    imu.c
  * @author  YANDLD
  * @version V2.5
  * @date    2015.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    
  ******************************************************************************
  */
#include "imu.h"



/*******************************************************************************
 * Defination
 ******************************************************************************/
#define PI             3.1415926f
#define Kp             12.0f     /* proportional gain governs rate of convergence to accelerometer/magnetometer */
#define Ki             0.000f     /* integral gain governs rate of convergence of gyroscope biases */

   
float halfT = 0.002f;

///* sliding filter */
//#if 0
//static uint32_t imu_sliding_filter(imu_raw_data_t raw_data, imu_raw_data_t * filter_data)
//{
//    int32_t sum_accel_x = 0;
//    int32_t sum_accel_y = 0;
//    int32_t sum_accel_z = 0;
//    int32_t sum_gyro_x = 0;
//    int32_t sum_gyro_y = 0;
//    int32_t sum_gyro_z = 0;
//    
//    /* fifo */
//    static int16_t fifo_accel_x[SLIDING_FILTER_DEEP];
//    static int16_t fifo_accel_y[SLIDING_FILTER_DEEP];
//    static int16_t fifo_accel_z[SLIDING_FILTER_DEEP];
//    static int16_t fifo_gyro_x[SLIDING_FILTER_DEEP];
//    static int16_t fifo_gyro_y[SLIDING_FILTER_DEEP];
//    static int16_t fifo_gyro_z[SLIDING_FILTER_DEEP];
//    
//    for(int i = SLIDING_FILTER_DEEP-1; i > 0; i--)
//    {
//        fifo_accel_x[i] =  fifo_accel_x[i-1];
//        fifo_accel_y[i] =  fifo_accel_y[i-1];
//        fifo_accel_z[i] =  fifo_accel_z[i-1];		
//        fifo_gyro_x[i] =  fifo_gyro_x[i-1];
//        fifo_gyro_y[i] =  fifo_gyro_y[i-1];
//        fifo_gyro_z[i] =  fifo_gyro_z[i-1];
//    }
//    
//    /* fifo input */
//    fifo_accel_x[0] = raw_data.ax;
//    fifo_accel_y[0] = raw_data.ay;
//    fifo_accel_z[0] = raw_data.az;
//    fifo_gyro_x[0] = raw_data.gx;
//    fifo_gyro_y[0] = raw_data.gy;
//    fifo_gyro_z[0] = raw_data.gz;
//    
//    /** fifo calculate */
//    for(int i=0; i < SLIDING_FILTER_DEEP; i++)
//    {
//        sum_accel_x += fifo_accel_x[i];
//        sum_accel_y += fifo_accel_y[i];
//        sum_accel_z += fifo_accel_z[i];
//        sum_gyro_x += fifo_gyro_x[i];
//        sum_gyro_y += fifo_gyro_y[i];
//        sum_gyro_z += fifo_gyro_z[i];	
//    }
//    filter_data->ax = sum_accel_x/SLIDING_FILTER_DEEP;
//    filter_data->ay = sum_accel_y/SLIDING_FILTER_DEEP; 
//    filter_data->az = sum_accel_z/SLIDING_FILTER_DEEP; 
//    filter_data->gx = sum_gyro_x/SLIDING_FILTER_DEEP;
//    filter_data->gy = sum_gyro_y/SLIDING_FILTER_DEEP; 
//    filter_data->gz = sum_gyro_z/SLIDING_FILTER_DEEP; 
//    filter_data->mx = raw_data.mx;
//    filter_data->my = raw_data.my;
//    filter_data->mz = raw_data.mz; 
//    return 0;
//}
//#endif


float invSqrt(float x) {
	/*float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));*/

	unsigned int i = 0x5F1F1412 - (*(unsigned int*)&x >> 1);
	float tmp = *(float*)&i;
	float y = tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
	return y;
	//return 1/sqrt(x);
}

//static inline float invSqrt(float x) 
//{
//	float halfx = 0.5f * x;
//	float y = x;
//	long i = *(long*)&y;
//	i = 0x5f3759df - (i>>1);
//	y = *(float*)&i;
//	y = y * (1.5f - (halfx * y * y));
//	return y;
//}

#define sampleFreq	200.0f		// sample frequency in Hz
#define betaDef		0.4f		// 2 * proportional gain

volatile float beta = betaDef;		
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;

#define twoKpDef	(2.0f * 1.0f)	// 2 * proportional gain
#define twoKiDef	(2.0f * 0.0f)	// 2 * integral gain

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
volatile float twoKp = twoKpDef;											// 2 * proportional gain (Kp)
volatile float twoKi = twoKiDef;											// 2 * integral gain (Ki)
//volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;					// quaternion of sensor frame relative to auxiliary frame
volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;	// integral error terms scaled by Ki


void MadgwickAHRSupdate(float gx,float gy,float gz,float ax,float ay,float az,float my,float mx,float mz, attitude_t * angle)
{
  float recipNorm;
  float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
  float halfex = 0.0f, halfey = 0.0f, halfez = 0.0f;
  float qa, qb, qc;

  // Auxiliary variables to avoid repeated arithmetic
  q0q0 = q0 * q0;
  q0q1 = q0 * q1;
  q0q2 = q0 * q2;
  q0q3 = q0 * q3;
  q1q1 = q1 * q1;
  q1q2 = q1 * q2;
  q1q3 = q1 * q3;
  q2q2 = q2 * q2;
  q2q3 = q2 * q3;
  q3q3 = q3 * q3;
    
    float hx, hy, bx, bz;
    float halfwx, halfwy, halfwz;
    
    // Normalise magnetometer measurement
    recipNorm = invSqrt(mx * mx + my * my + mz * mz);
    mx *= recipNorm;
    my *= recipNorm;
    mz *= recipNorm;
    
    // Reference direction of Earth's magnetic field
    hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
    hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
    bx = sqrt(hx * hx + hy * hy);
    bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));
    
    // Estimated direction of magnetic field
    halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
    halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
    halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);
    
    // Error is sum of cross product between estimated direction and measured direction of field vectors
    halfex = (my * halfwz - mz * halfwy);
    halfey = (mz * halfwx - mx * halfwz);
    halfez = (mx * halfwy - my * halfwx);


 // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if((ax != 0.0f) && (ay != 0.0f) && (az != 0.0f)) {
    float halfvx, halfvy, halfvz;
    
    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;
    
    // Estimated direction of gravity
    halfvx = q1q3 - q0q2;
    halfvy = q0q1 + q2q3;
    halfvz = q0q0 - 0.5f + q3q3;
  
    // Error is sum of cross product between estimated direction and measured direction of field vectors
    halfex += (ay * halfvz - az * halfvy);
    halfey += (az * halfvx - ax * halfvz);
    halfez += (ax * halfvy - ay * halfvx);
  }

  // Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
  if(halfex != 0.0f && halfey != 0.0f && halfez != 0.0f) {
    // Compute and apply integral feedback if enabled
    if(twoKi > 0.0f) {
      integralFBx += twoKi * halfex * (1.0f / sampleFreq);  // integral error scaled by Ki
      integralFBy += twoKi * halfey * (1.0f / sampleFreq);
      integralFBz += twoKi * halfez * (1.0f / sampleFreq);
      gx += integralFBx;  // apply integral feedback
      gy += integralFBy;
      gz += integralFBz;
    }
    else {
      integralFBx = 0.0f; // prevent integral windup
      integralFBy = 0.0f;
      integralFBz = 0.0f;
    }

    // Apply proportional feedback
    gx += twoKp * halfex;
    gy += twoKp * halfey;
    gz += twoKp * halfez;
  }
  
  // Integrate rate of change of quaternion
  gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
  gy *= (0.5f * (1.0f / sampleFreq));
  gz *= (0.5f * (1.0f / sampleFreq));
  qa = q0;
  qb = q1;
  qc = q2;
  q0 += (-qb * gx - qc * gy - q3 * gz);
  q1 += (qa * gx + qc * gz - q3 * gy);
  q2 += (qa * gy - qb * gz + q3 * gx);
  q3 += (qa * gz + qb * gy - qc * gx);
  
  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
  
  
    /* output data */
    angle->Y = atan2(2 * q1 * q2 + 2 * q0 * q3, q0*q0+q1*q1-q2*q2-q3*q3)* 57.3; 
    angle->P = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;																			// pitcho???
    angle->R = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
    
}


////!< the mx my mz order are related to PCB layout!!
//static inline void updateAHRS(float gx,float gy,float gz,float ax,float ay,float az,float my,float mx,float mz, attitude_t * angle)
//{
//    float norm = 0;
//    float hx = 0, hy = 0, hz = 0, bx = 0, bz = 0;			
//    float vx = 0, vy = 0, vz = 0, wx = 0, wy = 0, wz = 0;		
//    float ex = 0, ey = 0, ez = 0;
//    static float q0 = 1; 
//    static float q1 = 0; 
//    static float q2 = 0;
//    static float q3 = 0;
//    static float exInt = 0, eyInt = 0, ezInt = 0;

//    float q0q0 = q0*q0;
//    float q0q1 = q0*q1;
//    float q0q2 = q0*q2;
//    float q0q3 = q0*q3;
//    float q1q1 = q1*q1;
//    float q1q2 = q1*q2;
//    float q1q3 = q1*q3;
//    float q2q2 = q2*q2;
//    float q2q3 = q2*q3;
//    float q3q3 = q3*q3;

//    if(ax*ay*az==0)
//    {
//        return;    
//    }
//		
//    
//    norm = invSqrt(ax*ax + ay*ay + az*az);
//    ax = ax * norm;
//    ay = ay * norm;
//    az = az * norm;

//    norm = invSqrt(mx*mx + my*my + mz*mz);
//    mx = mx * norm;
//    my = my * norm;
//    mz = mz * norm;
//	
//    /* compute reference direction of flux */
//    hx = 2*mx*(0.5f - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
//    hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5f - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
//    hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5f - q1q1 - q2q2);         

//    bx = 1/invSqrt((hx*hx) + (hy*hy));
//    bz = hz; 
//	
//    /* estimated direction of gravity and flux (v and w) */
//    vx = 2*(q1q3 - q0q2);
//    vy = 2*(q0q1 + q2q3);
//    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

//    wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
//    wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
//    wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2);
//    
//    /* error is sum of cross product between reference direction of fields and direction measured by sensors */
//    ex = (ay*vz - az*vy) + (my*wz - mz*wy);
//    ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
//    ez = (ax*vy - ay*vx) + (mx*wy - my*wx);

//    exInt = exInt + ex * Ki;
//    eyInt = eyInt + ey * Ki;
//    ezInt = ezInt + ez * Ki;

//    /* adjusted gyroscope measurements */
//    gx = gx + Kp*ex + exInt;
//    gy = gy + Kp*ey + eyInt;
//    gz = gz + Kp*ez + ezInt;

//    /* integrate quaternion rate and normalise */
//    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
//    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
//    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
//    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

//    /* normalise quaternion */
//    norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
//    q0 = q0 * norm;
//    q1 = q1 * norm;
//    q2 = q2 * norm;
//    q3 = q3 * norm;
//    
//    /* output data */
//    angle->Y = atan2(2 * q1 * q2 + 2 * q0 * q3, q0*q0+q1*q1-q2*q2-q3*q3)* 57.3; 
//    angle->P = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;																			// pitcho???
//    angle->R = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
//}

//!< this functino must be called about every 2ms to get accurate eular angles

uint32_t imu_get_euler_angle(float *adata, float *gdata, float *mdata, attitude_t *angle)
{

    MadgwickAHRSupdate( (float)gdata[0]*PI/180,
                (float)gdata[1]*PI/180,
                (float)gdata[2]*PI/180,
                (float)adata[0],
                (float)adata[1],
                (float)adata[2],
                (float)mdata[0],
                (float)mdata[1],
                -(float)mdata[2],
                angle);
    return 0;
}




