#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>



typedef __packed struct
{
    
    int16_t acc[3];
    int16_t gyo[3];
    int16_t mag[3];
    int16_t R;
    int16_t P;
    int16_t Y;
    int32_t pressure; /* pressure */
}payload_t;


//!< API

uint32_t ano_encode(payload_t* data, uint8_t* buf);

#endif


