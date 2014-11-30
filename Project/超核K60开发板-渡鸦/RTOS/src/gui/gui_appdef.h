#ifndef __GUI_APPDEF_H__
#define __GUI_APPDEF_H__

#include <rtthread.h>
#include "GUI.H"
#include "WM.H"
extern rt_mq_t guimq;

typedef struct
{
    int cmd;
    void (*exec)(void *parameter);
    void *parameter;
}gui_msg_t;

//!< API below API should only call in GUI thread
void GUI_AppDispImage(const char * path);
void GUI_AppDispCalender(void);
void GUI_AppDispTime(void);
const char *GUI_AooDispChooseFile(void);
WM_HWIN GUI_AppDispSysInfo(void);

#endif



