#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>


enum
{
    kPTL_DATA_OUTPUT = 0xAF, /* 0xAF */
    kPTL_REQ_FW,
    kPTL_DATA_FW,
    kPTL_REQ_RAW,
    kPTL_DATA_RAW,
    kPTL_REQ_OFS_ALL,
    kPTL_REQ_OFS_ACC,
    kPTL_REQ_OFS_GYRO,
    kPTL_REQ_OFS_MAG,
    kPTL_REQ_SAVE_OFS,
    kPTL_DATA_OFS_ALL,
    kPTL_DATA_OFS_ACC,
    kPTL_DATA_OFS_GYRO,
    kPTL_DATA_OFS_MAG,
    
    kPTL_REQ_MODE_RUN,
    kPTL_REQ_MODE_CAL,
};


typedef __packed struct
{
    int16_t acc[3];
    int16_t gyo[3];
    int16_t mag[3];
    int16_t R;
    int16_t P;
    int16_t Y;
    int32_t pressure;
}payload_t;

typedef __packed struct
{
    uint8_t version;
    uint32_t uid;
}fw_info_t;

typedef __packed struct
{
    int16_t acc_offset[3];
    int16_t gyro_offset[3];
    int16_t mag_offset[3];
}offset_t;

typedef struct
{
    uint8_t buf[32];
    uint8_t len;
    uint8_t cmd;
}rev_data_t;


//!< API

uint32_t ano_encode_packet(payload_t* data, uint8_t* buf);
uint32_t ano_encode_fwinfo(fw_info_t* fwinfo, uint8_t* buf);
uint32_t ano_encode_offset_packet(offset_t* offset, uint8_t* buf);
int ano_rec(uint8_t ch, rev_data_t *rd);


#endif


