#include "protocol.h"
#include <string.h>

#ifndef BSWAP_16
#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif

typedef __packed struct
{
    uint8_t trans_header[3];
    transmit_user_data user_data;
    uint8_t trans_reserved[4];
    uint8_t sum;
}trans_packet_t;

uint32_t user_data2buffer(transmit_user_data* data, uint8_t* buf)
{
    int i;
    uint8_t sum = 0;
    trans_packet_t *packet = (trans_packet_t*)buf;
    uint8_t *p = buf;
    for(i=0;i<3;i++)
    {
        data->trans_accel[i] = BSWAP_16(data->trans_accel[i]);
        data->trans_gyro[i] = BSWAP_16(data->trans_gyro[i]);
        data->trans_mag[i] = BSWAP_16(data->trans_mag[i]);
    }
    data->trans_yaw = BSWAP_16(data->trans_yaw);
    data->trans_pitch = BSWAP_16(data->trans_pitch);
    data->trans_roll = BSWAP_16(data->trans_roll);
    memcpy(&packet->user_data, data, sizeof(transmit_user_data));
    packet->trans_header[0] = 0x88;
    packet->trans_header[1] = 0xAF;
    packet->trans_header[2] = 0x1C;
    packet->trans_reserved[0] = 0x00;
    packet->trans_reserved[1] = 0x00;
    packet->trans_reserved[2] = 0x00;
    packet->trans_reserved[3] = 0x00;
    
    for(i = 0; i < sizeof(trans_packet_t) - 1;i++)
    {
      sum += *p++;
    }
    packet->sum = sum;
    return sizeof(trans_packet_t);
}



