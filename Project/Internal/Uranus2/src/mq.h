#ifndef __BM_MQ_H__
#define __BM_MQ_H__

#ifdef __cplusplus
extern "C" {
#endif
    

#include <stdint.h>

typedef struct
{
    uint8_t   cmd;
    uint8_t   type;
    uint16_t  msg_len;
    void*     msg;
}msg_t;




void mq_init(void);
uint8_t mq_exist(void);
msg_t *mq_pop(void);
uint8_t mq_push(msg_t pMsg);

#ifdef __cplusplus
}
#endif


#endif
