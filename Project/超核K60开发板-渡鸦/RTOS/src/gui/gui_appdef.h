#ifndef __GUI_APPDEF_H__
#define __GUI_APPDEF_H__

#include <rtthread.h>
extern rt_mq_t guimq;

typedef struct
{
    int cmd;
    void (*exec)(void);
}gui_msg_t;


#endif



