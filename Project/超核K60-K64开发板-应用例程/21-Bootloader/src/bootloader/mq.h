#ifndef MESSAGE_MANAGE_H_INCLUDED
#define MESSAGE_MANAGE_H_INCLUDED

#include <stdint.h>

typedef struct
{
    uint8_t  cmd;
    uint8_t  type;
    uint16_t len;
    void*    pMessage;
} msg_t;


void mq_init(void);
uint8_t mq_exist(void);
msg_t *mq_pop(void);
uint8_t mq_push(msg_t msg);



#endif // MESSAGE_MANAGE_H_INCLUDED
