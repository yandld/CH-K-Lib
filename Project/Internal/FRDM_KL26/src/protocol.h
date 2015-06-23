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

typedef struct
{
    uint8_t buf[32];
    uint8_t len;
}rev_data_t;


//!< API

uint32_t ano_encode(payload_t* data, uint8_t* buf);
int ano_rec(uint8_t ch, rev_data_t *rd);
#endif


