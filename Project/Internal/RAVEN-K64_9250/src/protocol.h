#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>



typedef __packed struct
{
    int16_t trans_accel[3];
    int16_t trans_gyro[3];
    int16_t trans_mag[3];
    int16_t trans_roll;
    int16_t trans_pitch;
    int16_t trans_yaw;
    int32_t trans_pressure;
}payload_t;


//!< API

uint32_t ano_encode(payload_t* data, uint8_t* buf);

#endif


