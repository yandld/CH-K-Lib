#include "protocol.h"
#include <string.h>

#ifndef BSWAP_16
#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif


#ifndef BSWAP_32
#define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))
#endif

static callback_t ano_callback = NULL;


enum ano_status
{
    kANO_IDLE,
    kANO_CMD,
    kANO_LEN,
    kANO_DATA,
    kANO_CHECKSUM,
};

uint32_t ano_encode_fwinfo(fw_info_t* fwinfo, uint8_t* buf)
{
    int i;
    uint8_t sum = 0;
    uint8_t len;
    uint8_t *p = buf;
    
    len = sizeof(fw_info_t);
    
    buf[0] = 0x88;
    buf[1] = kPTL_DATA_FW;
    buf[2] = len;
    buf[3] = fwinfo->version;
    buf[4] = fwinfo->uid>>0;
    buf[5] = fwinfo->uid>>8;
    buf[6] = fwinfo->uid>>16;
    buf[7] = fwinfo->uid>>24;
    buf[8] = fwinfo->id;
    buf[9] = fwinfo->mode; 
    for(i = 0; i < len+3; i++)
    {
      sum += *p++;
    }
    buf[len+3] = sum;
    return (len+4);
}

uint32_t ano_encode_packet(payload_t* payload, uint8_t* buf)
{
    int i;
    uint8_t sum = 0;
    uint8_t len;
    uint8_t *p = buf;
    
    len = sizeof(payload_t);
    
    buf[0] = 0x88;
    buf[1] = kPTL_DATA_OUTPUT;
    buf[2] = len;
    buf[3] = (payload->acc[0])>>8;
    buf[4] = (payload->acc[0])>>0;
    buf[5] = (payload->acc[1])>>8;
    buf[6] = (payload->acc[1])>>0;
    buf[7] = (payload->acc[2])>>8;
    buf[8] = (payload->acc[2])>>0;
    buf[9] = (payload->gyo[0])>>8;
    buf[10] = (payload->gyo[0])>>0;
    buf[11] = (payload->gyo[1])>>8;
    buf[12] = (payload->gyo[1])>>0;
    buf[13] = (payload->gyo[2])>>8;
    buf[14] = (payload->gyo[2])>>0;
    buf[15] = (payload->mag[0])>>8;
    buf[16] = (payload->mag[0])>>0;
    buf[17] = (payload->mag[1])>>8;
    buf[18] = (payload->mag[1])>>0;
    buf[19] = (payload->mag[2])>>8;
    buf[20] = (payload->mag[2])>>0;
    buf[21] = (payload->P)>>8;
    buf[22] = (payload->P)>>0;
    buf[23] = (payload->R)>>8;
    buf[24] = (payload->R)>>0;
    buf[25] = (payload->Y)>>8;
    buf[26] = (payload->Y)>>0;
    
    buf[27] = (payload->pressure)>>0;
    buf[28] = (payload->pressure)>>8;
    buf[29] = (payload->pressure)>>16;
    buf[30] = (payload->pressure)>>24;
    
    for(i = 0; i < len+3;i++)
    {
      sum += *p++;
    }
    buf[len+3] = sum;
    return (len+4);
}

uint32_t ano_encode_offset_packet(offset_t* offset, uint8_t* buf)
{
    int i;
    uint8_t sum = 0;
    uint8_t len;
    uint8_t *p = buf;
    
    len = sizeof(offset_t);
    
    buf[0] = 0x88;
    buf[1] = kPTL_DATA_OFS_ALL;
    buf[2] = len;
    buf[3] = (offset->acc_offset[0])>>8;
    buf[4] = (offset->acc_offset[0])>>0;
    buf[5] = (offset->acc_offset[1])>>8;
    buf[6] = (offset->acc_offset[1])>>0;
    buf[7] = (offset->acc_offset[2])>>8;
    buf[8] = (offset->acc_offset[2])>>0;
    buf[9] = (offset->gyro_offset[0])>>8;
    buf[10] = (offset->gyro_offset[0])>>0;
    buf[11] = (offset->gyro_offset[1])>>8;
    buf[12] = (offset->gyro_offset[1])>>0;
    buf[13] = (offset->gyro_offset[2])>>8;
    buf[14] = (offset->gyro_offset[2])>>0;
    buf[15] = (offset->mag_offset[0])>>8;
    buf[16] = (offset->mag_offset[0])>>0;
    buf[17] = (offset->mag_offset[1])>>8;
    buf[18] = (offset->mag_offset[1])>>0;
    buf[19] = (offset->mag_offset[2])>>8;
    buf[20] = (offset->mag_offset[2])>>0;
    
    for(i = 0; i < len+3;i++)
    {
      sum += *p++;
    }
    buf[len+3] = sum;
    return (len+4);
}



void ano_set_callback(callback_t cb)
{
    ano_callback = cb;
}

int ano_rec(rev_data_t *rd, uint8_t *buf, uint32_t len)
{
    int ret;
    int i;
    static int j;
    static enum ano_status states = kANO_IDLE;
    
    ret = 1;
    for(i=0; i<len; i++)
    {
        switch(states)
        {
            case kANO_IDLE:
                if((uint8_t)buf[i] == 0x88)
                {
                    states = kANO_CMD;
                }
                break;
            case kANO_CMD:
                rd->cmd = buf[i];
                states = kANO_LEN;
                break;
            case kANO_LEN:
                rd->len = buf[i];
                if(rd->len == 0)
                {
                    states = kANO_CHECKSUM;
                }
                else
                {
                    j = 0;
                    states = kANO_DATA;
                }
                break;
            case kANO_DATA:
                rd->buf[j++] = buf[i];
                if(j == rd->len)
                {
                    states = kANO_CHECKSUM;
                }
                break;
            case kANO_CHECKSUM:
                if(ano_callback)
                {
                    ano_callback(rd);
                }

                ret = 0;
                states = kANO_IDLE;
                break;
        }
    }
    return ret;
}


