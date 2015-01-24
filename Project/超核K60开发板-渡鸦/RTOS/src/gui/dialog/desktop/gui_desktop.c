#include "gui_appdef.h"


#define ID_WINDOW_0     (GUI_ID_USER + 0x01)
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_ICONVIEW_0   (GUI_ID_USER + 0x01)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
#define ID_BUTTON_1     (GUI_ID_USER + 0x02)
#define ID_BUTTON_2     (GUI_ID_USER + 0x03)
#define ID_BUTTON_3     (GUI_ID_USER + 0x04)
#define ID_BUTTON_4     (GUI_ID_USER + 0x05)
#define ID_BUTTON_5     (GUI_ID_USER + 0x06)
#define ID_IMAGE_0      (GUI_ID_USER + 0x04)


static UIAppEntry UIApp[] = 
{
    {"calendar",    "calendar",     "/SF/SYS/GAME.BMP", RT_NULL, 10, 10, 50, 50,         ID_BUTTON_0, GUI_AppDispCalender},
    {"system",      "system",       "/SF/SYS/GAME.BMP", RT_NULL, 10, 10, 50, 50,         ID_BUTTON_1, GUI_AppDispSysInfo},
    {"file",        "file",         "/SF/SYS/GAME.BMP", RT_NULL, 10, 10, 50, 50,         ID_BUTTON_2, GUI_AooDispChooseFile},
    {"clock",       "clock",        "/SF/SYS/GAME.BMP", RT_NULL, 10, 10, 50, 50,         ID_BUTTON_3, GUI_AppDispTime},
    {"calibration", "calibration",  "/SF/SYS/GAME.BMP", RT_NULL, 10, 10, 50, 50,         ID_BUTTON_4, GUI_ExecCalibrationDialog},
    {"thread",      "thread",       "/SF/SYS/GAME.BMP", RT_NULL, 10, 10, 50, 50,         ID_BUTTON_5, GUI_CreateTaskerDialog},
};



static const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
    { FRAMEWIN_CreateIndirect,    "Framewin", ID_FRAMEWIN_0, 0, 0, 500, 325, 0, 0, 0 },
};
    
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 320, 240, 0, 0x0, 0 },
 //   { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 240, 320, 0, IMAGE_CF_MEMDEV | IMAGE_CF_ATTACHED | IMAGE_CF_TILE | IMAGE_CF_ALPHA, 0 },
};



static void _cbDialog2(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;
    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
        hItem = pMsg->hWin;
        FRAMEWIN_SetBorderSize(hItem, 0);
        FRAMEWIN_SetTitleVis(hItem, 0);
        FRAMEWIN_SetClientColor(hItem, GUI_LIGHTBLUE);
        break;
        default:
        WM_DefaultProc(pMsg);
        break;
    }    
    
};


static void _cbDialog(WM_MESSAGE * pMsg) {
    int i;
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    gui_msg_t msg;
    msg.exec = RT_NULL;
    const GUI_PID_STATE * pState;
    
    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
        hItem = pMsg->hWin;
        WM_EnableMemdev(hItem);
        
        WM_MOTION_SetMoveable(hItem, WM_CF_MOTION_X, 1);
        WINDOW_SetBkColor(hItem, GUI_LIGHTBLUE);
        int fd; rt_uint8_t *p[6];
        
        for(i=0;i<GUI_COUNTOF(UIApp);i++)
        {
            hItem = BUTTON_CreateAsChild(10, 10, 50, 50, pMsg->hWin, UIApp[i].GUID, WM_CF_SHOW);
            BUTTON_SetText(hItem, UIApp[i].text);
            WM_EnableMemdev(hItem);
			WM_MoveTo(hItem, 10+(i%3)*70, (i/3)*70 + 17);
            UIApp[i].plogo = rt_malloc(9*1024);
            fd = open(UIApp[i].logoPath, O_RDONLY , 0);
            read(fd, p[i], 8192);
            BUTTON_SetBMPEx(hItem, BUTTON_BI_UNPRESSED, UIApp[i].plogo ,0, 0);
            close(fd);
        }
        
//        p[2] = rt_malloc(38399);
//        fd = open("/SD/DESKTOP.JPG",O_RDONLY , 0);
//        read(fd, p[2], 38399);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
//        WM_EnableMemdev(hItem);
//          IMAGE_SetJPEG(hItem, p[2], 38399);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
    if(NCode == WM_NOTIFICATION_RELEASED)
    {
        for(i=0;i<GUI_COUNTOF(UIApp);i++)
        {
            if(Id == UIApp[i].GUID)
            {
                msg.exec = UIApp[i].exec;
                break;
            }
        }
        if(msg.exec != RT_NULL)
        {
            rt_mq_send(guimq, &msg, sizeof(msg));
        }
    }
    break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

  
WM_HWIN GUI_CreateDesktopDialog(void)
{
    int i;
    int fd;
    WM_HWIN  hWin;
    WM_MOTION_Enable(1);

    hWin = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog2, WM_HBKWIN, -100, -2);
    GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin, 0, 0);
}



