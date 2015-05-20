#include "protocol.h"
#include <string.h>

#ifndef BSWAP_16
#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif


#ifndef BSWAP_32
#define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))
#endif

typedef __packed struct
{
    uint8_t trans_header[3];
    payload_t patload;
    uint8_t sum;
}packet_t;


uint32_t ano_encode(payload_t* payload, uint8_t* buf)
{
    int i;
    uint8_t sum = 0;
    packet_t *packet = (packet_t*)buf;
    uint8_t *p = buf;
    for(i=0;i<3;i++)
    {
        payload->acc[i] = BSWAP_16(payload->acc[i]);
        payload->gyo[i] = BSWAP_16(payload->gyo[i]);
        payload->mag[i] = BSWAP_16(payload->mag[i]);
    }
    payload->Y = BSWAP_16(payload->Y);
    payload->P = BSWAP_16(payload->P);
    payload->R = BSWAP_16(payload->R);
    payload->pressure = (payload->pressure);
    memcpy(&packet->patload, payload, sizeof(payload_t));
    packet->trans_header[0] = 0x88;
    packet->trans_header[1] = 0xAF;
    packet->trans_header[2] = 0x1C;
    for(i = 0; i < sizeof(packet_t)-1;i++)
    {
      sum += *p++;
    }
    packet->sum = sum;
    return sizeof(packet_t);
}



