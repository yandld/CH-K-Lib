#ifndef __SYS_DEF_H__
#define __SYS_DEF_H__

#include <stdint.h>


typedef struct
{
    uint32_t type;
    uint32_t cmd;
    uint8_t *msg;
    uint8_t payload[32];
}sys_msg;




#endif

