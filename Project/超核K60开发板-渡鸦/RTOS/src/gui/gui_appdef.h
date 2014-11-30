#ifndef __GUI_APPDEF_H__
#define __GUI_APPDEF_H__

#include <rtthread.h>
extern rt_mq_t guimq;

typedef struct
{
    int cmd;
    void (*exec)(void *parameter);
    void *parameter;
}gui_msg_t;

//!< API below API should only call in GUI thread
void GUI_AppDispImage(const char * path);

#endif



