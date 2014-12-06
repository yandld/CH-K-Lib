#ifndef __GUI_APPDEF_H__
#define __GUI_APPDEF_H__

#include <rtthread.h>
#include "GUI.H"
#include "WM.H"


typedef enum
{
    kJPEG,
    kJPG,
    kTXT,
    kPNG,
    kMD,
    kINI,
    kDOC,
    kBMP,
    kAVI,
    kWMV,
    kBIN,
    kHEX,
    kFileNameMax,
}GUI_FileType;

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
void GUI_AppNotepad(void);
const char *GUI_AooDispChooseFile(void);
WM_HWIN GUI_AppDispSysInfo(void);
GUI_FileType GUI_AppGetFileType(const char * fileName);


#endif



