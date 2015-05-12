
#ifndef __PARSE_CMD_H__
#define __PARSE_CMD_H__


#include <stdint.h>
#include <stdbool.h>




struct cmd_handler
{
    int (*cmd_cal_accel)(int16_t x, int16_t y, int16_t z);
    int (*cmd_cal_gyro)(int16_t x, int16_t y, int16_t z);
    int (*cmd_read_cal)(int16_t);
};

typedef struct imu_rev_init* imu_rev_init_t;


#endif

