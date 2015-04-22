/**
  ******************************************************************************
  * @file    mpu9250.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __MPU9250_H__
#define __MPU9250_H__

#include <stdint.h>
#include <stdbool.h>

enum accel_scale
{
    AFS_2G,
    AFS_4G,
    AFS_8G,
    AFS_16G
};

enum gyro_scale
{
    GFS_250DPS,
    GFS_500DPS,
    GFS_1000DPS,
    GFS_2000DPS
};

enum meg_scale
{
    MFS_14BITS = 0, // 0.6 mG per LSB
    MFS_16BITS      // 0.15 mG per LSB
};

struct mpu_config
{
    enum accel_scale        afs;
    enum gyro_scale         gfs;
    enum meg_scale          mfs;
    bool                    enable_aself_test;
};



#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

//!< API function
int mpu9250_init(uint32_t instance);
int mpu9250_read_accel(int16_t* x, int16_t* y, int16_t* z);
int mpu9250_read_gyro(int16_t* x, int16_t* y, int16_t* z);
int mpu9250_read_mag(int16_t* x, int16_t* y, int16_t* z);
int mpu9250_config(struct mpu_config *config);


#endif
