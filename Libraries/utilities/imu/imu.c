#include "imu.h"



/*******************************************************************************
 * Defination
 ******************************************************************************/
#define PI             3.14159
#define Kp             100.0f     // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki             0.008f     // integral gain governs rate of convergence of gyroscope biases
#define halfT          0.002f
#define Gyro_G         0.0610351
#define Gyro_Gr        0.0010653
#define SLIDING_FILTER_DEEP  5
   


typedef struct 
{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float mx;
    float my;
    float mz;
}imu_float_data_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/********************************************************************************
 * Variables
 ******************************************************************************/
static imu_io_install_t * gpIOInstallStruct;   /* install struct	*/
 /*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t imu_io_install(imu_io_install_t * IOInstallStruct)
{
    gpIOInstallStruct = IOInstallStruct;
    return 0;
}


static uint32_t imu_sliding_filter(imu_raw_data_t raw_data, imu_raw_data_t * filter_data)
{
    int32_t sum_accel_x =  0;
    int32_t sum_accel_y =  0;
    int32_t sum_accel_z =  0;
    int32_t sum_gyro_x =   0;
    int32_t sum_gyro_y =  0;
    int32_t sum_gyro_z =  0;
    int32_t sum_magn_x = 0;
    int32_t sum_magn_y = 0;
    int32_t sum_magn_z = 0;
    //fifo
    static int16_t fifo_accel_x[SLIDING_FILTER_DEEP];
    static int16_t fifo_accel_y[SLIDING_FILTER_DEEP];
    static int16_t fifo_accel_z[SLIDING_FILTER_DEEP];
    static int16_t fifo_gyro_x[SLIDING_FILTER_DEEP];
    static int16_t fifo_gyro_y[SLIDING_FILTER_DEEP];
    static int16_t fifo_gyro_z[SLIDING_FILTER_DEEP];
    static int16_t fifo_magn_x[SLIDING_FILTER_DEEP];
    static int16_t fifo_magn_y[SLIDING_FILTER_DEEP];
    static int16_t fifo_magn_z[SLIDING_FILTER_DEEP];
    
    for(int i=9;i>0;i--)
    {
        fifo_accel_x[i] =  fifo_accel_x[i-1];
        fifo_accel_y[i] =  fifo_accel_y[i-1];
        fifo_accel_z[i] =  fifo_accel_z[i-1];		
        fifo_gyro_x[i] =  fifo_gyro_x[i-1];
        fifo_gyro_y[i] =  fifo_gyro_y[i-1];
        fifo_gyro_z[i] =  fifo_gyro_z[i-1];
        fifo_magn_x[i] =  fifo_magn_x[i-1];
        fifo_magn_y[i] =  fifo_magn_y[i-1];
        fifo_magn_z[i] =  fifo_magn_z[i-1];
    }
    /*fifo input*/
    fifo_accel_x[0] = raw_data.ax;
    fifo_accel_y[0] = raw_data.ay;
    fifo_accel_z[0] = raw_data.az;
    fifo_gyro_x[0] = raw_data.gx;
    fifo_gyro_y[0] = raw_data.gy;
    fifo_gyro_z[0] = raw_data.gz;
    fifo_magn_x[0] = raw_data.mx;
    fifo_magn_y[0] = raw_data.my;
    fifo_magn_z[0] = raw_data.mz;
    /**fifo calculate*/
    for(int i=0;i<10;i++)
    {
        sum_accel_x += fifo_accel_x[i];
        sum_accel_y += fifo_accel_y[i];
        sum_accel_z += fifo_accel_z[i];
        sum_gyro_x += fifo_gyro_x[i];
        sum_gyro_y += fifo_gyro_y[i];
        sum_gyro_z += fifo_gyro_z[i];	
        sum_magn_x += fifo_magn_x[i];
        sum_magn_y += fifo_magn_y[i];
        sum_magn_z += fifo_magn_z[i];	
    }
    filter_data->ax = sum_accel_x/SLIDING_FILTER_DEEP;
    filter_data->ay = sum_accel_y/SLIDING_FILTER_DEEP; 
    filter_data->az = sum_accel_z/SLIDING_FILTER_DEEP; 
    filter_data->gx = sum_gyro_x/SLIDING_FILTER_DEEP;
    filter_data->gy = sum_gyro_y/SLIDING_FILTER_DEEP; 
    filter_data->gz = sum_gyro_z/SLIDING_FILTER_DEEP; 
    filter_data->mx = raw_data.mx;
    filter_data->my = raw_data.my;
    filter_data->mz = raw_data.mz; 
    return 0;
}


//!< format data into float value
static uint32_t imu_format_data(imu_raw_data_t * raw_data, imu_float_data_t * float_data)
{
    float_data->ax = (float)raw_data->ax;
    float_data->ay = (float)raw_data->ay;
    float_data->az = (float)raw_data->az;
    
    float_data->gx = (float)raw_data->gx;
    float_data->gy = (float)raw_data->gy;
    float_data->gz = (float)raw_data->gz;
    
    float_data->mx = (float)raw_data->mx;
    float_data->my = (float)raw_data->my;
    float_data->mz = (float)raw_data->mz;
    return 0;
}

//!< the mx my mz order are related to PCB layout!!
void updateAHRS(float gx,float gy,float gz,float ax,float ay,float az,float mx,float mz,float my, imu_float_euler_angle_t * angle)
{
    float norm = 0;
    float hx = 0, hy = 0, hz = 0, bx = 0, bz = 0;			
    float vx = 0, vy = 0, vz = 0, wx = 0, wy = 0, wz = 0;		
    float ex = 0, ey = 0, ez = 0;
    static float q0 = 1; 
    static float q1 = 0; 
    static float q2 = 0;
    static float q3 = 0;
    static float exInt = 0, eyInt = 0, ezInt = 0;

    float q0q0 = q0*q0;
    float q0q1 = q0*q1;
    float q0q2 = q0*q2;
    float q0q3 = q0*q3;
    float q1q1 = q1*q1;
    float q1q2 = q1*q2;
    float q1q3 = q1*q3;
    float q2q2 = q2*q2;
    float q2q3 = q2*q3;
    float q3q3 = q3*q3;

    if(ax*ay*az==0)
    {
        return;    
    }
		
    norm = sqrt(ax*ax + ay*ay + az*az);
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    norm = sqrt(mx*mx + my*my + mz*mz);
    mx = mx / norm;
    my = my / norm;
    mz = mz / norm;
	
    /* compute reference direction of flux */
    hx = 2*mx*(0.5f - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
    hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5f - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
    hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5f - q1q1 - q2q2);         

    bx = sqrt((hx*hx) + (hy*hy));
    bz = hz; 
	
    /* estimated direction of gravity and flux (v and w) */
    vx = 2*(q1q3 - q0q2);
    vy = 2*(q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
    wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
    wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2);  
    /* error is sum of cross product between reference direction of fields and direction measured by sensors */
    ex = (ay*vz - az*vy) + (my*wz - mz*wy);
    ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
    ez = (ax*vy - ay*vx) + (mx*wy - my*wx);

    exInt = exInt + ex * Ki;
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    /* adjusted gyroscope measurements */
    gx = gx + Kp*ex + exInt;
    gy = gy + Kp*ey + eyInt;
    gz = gz + Kp*ez + ezInt;

    /* integrate quaternion rate and normalise */
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

    /* normalise quaternion */
    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;
    /* output data */
    angle->imu_yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3; 
    angle->imu_pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;																			// pitcho???
    angle->imu_roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
}

//!< this functino must be called about every 2ms to get accurate eular angles
uint32_t imu_get_euler_angle(imu_float_euler_angle_t * angle, imu_raw_data_t * raw_data)
{
    uint8_t ret = 0;
    int16_t ax,ay,az,gx,gy,gz,mx,my,mz;
    imu_raw_data_t filter_data;
    imu_float_data_t float_data;
    ret += gpIOInstallStruct->imu_get_accel(&ax, &ay, &az);
    ret += gpIOInstallStruct->imu_get_gyro(&gx, &gy, &gz); 
    ret += gpIOInstallStruct->imu_get_mag(&mx, &my, &mz);
    if(ret >0)
    {
      return ret;
    }
    raw_data->ax = ax;
    raw_data->ay = ay;
    raw_data->az = az;
    raw_data->gx = gx;
    raw_data->gy = gy;
    raw_data->gz = gz;
    raw_data->mx = mx;
    raw_data->my = my;
    raw_data->mz = mz;

    //I need rawdata I give you filtered data
    imu_sliding_filter(*raw_data, &filter_data);

    // I need filtered data I give you float data
    imu_format_data(raw_data, &float_data);
    //I need float data I give you euler angles
    updateAHRS( float_data.gx * Gyro_Gr,
                float_data.gy * Gyro_Gr,
                float_data.gz * Gyro_Gr,
                float_data.ax,
                float_data.ay,
                float_data.az,
                float_data.mx,
                float_data.my,
                float_data.mz,
                angle);
    return 0;
}




