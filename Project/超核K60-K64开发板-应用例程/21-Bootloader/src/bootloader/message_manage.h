#ifndef MESSAGE_MANAGE_H_INCLUDED
#define MESSAGE_MANAGE_H_INCLUDED

#include <stdint.h>

typedef struct
{
    uint8_t  m_Command;
    uint8_t  m_MessageType;
    uint16_t m_MsgLen;
    void*    pMessage;
} MessageType_t;

extern void fn_queue_init(void);
extern uint8_t fn_msg_exist(void);
extern MessageType_t *fn_msg_pop(void);
extern uint8_t fn_msg_push(MessageType_t msg);







#endif // MESSAGE_MANAGE_H_INCLUDED
