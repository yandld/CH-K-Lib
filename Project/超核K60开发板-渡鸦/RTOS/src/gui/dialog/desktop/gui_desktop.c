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


static const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
    { FRAMEWIN_CreateIndirect,    "Framewin", ID_FRAMEWIN_0, 0, 0, 500, 325, 0, 0, 0 },
};
    
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 320, 240, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 240, 320, 0, IMAGE_CF_MEMDEV | IMAGE_CF_ATTACHED | IMAGE_CF_TILE | IMAGE_CF_ALPHA, 0 },
    { BUTTON_CreateIndirect, "Cal",           ID_BUTTON_0, 80, 10, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Info",          ID_BUTTON_1, 0, 10, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Time",          ID_BUTTON_2, 0, 80, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "File",          ID_BUTTON_3, 6, 49, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Cali",          ID_BUTTON_4, 71, 49, 50, 50, 0, 0x0, 0 },
 // { BUTTON_CreateIndirect, "Test",          ID_BUTTON_5, 137, 49, 50, 50, 0, 0x0, 0 },
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
        case WM_PAINT:
        
        break;
        default:
        WM_DefaultProc(pMsg);
        break;
    }    
    
};

static char *AppImageTable[] = 
{
    "/SF/SYS/GAME.BMP",
    "/SF/SYS/WIFI.BMP",
    "/SF/SYS/FILE.BMP",
    "/SF/SYS/PER.BMP",
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
        
//        p[2] = rt_malloc(38399);
//        fd = open("/SD/DESKTOP.JPG",O_RDONLY , 0);
//        read(fd, p[2], 38399);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
//        WM_EnableMemdev(hItem);
//          IMAGE_SetJPEG(hItem, p[2], 38399);


		for (i=0;i<5;i++)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0 + i);
            WM_EnableMemdev(hItem);
			WM_MoveTo(hItem, 10+(i%4)*60, (i/4)*70 + 10);
            
            if(AppImageTable[i])
            {
                p[i] = rt_malloc(9*1024);
                fd = open(AppImageTable[i],O_RDONLY , 0);
                read(fd, p[i], 8192);
                BUTTON_SetBMPEx(hItem, BUTTON_BI_UNPRESSED, p[i] ,0, 0);
                close(fd);
            }
		}
        hItem = pMsg->hWin;
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
    if(NCode == WM_NOTIFICATION_RELEASED)
    {
        switch (Id)
        {
            case ID_BUTTON_0:
                msg.exec = GUI_AppDispCalender;
                break;
            case ID_BUTTON_1:
                msg.exec = GUI_AppDispSysInfo;
                break;
            case ID_BUTTON_2:
                msg.exec = GUI_AppDispTime;
                break;
            case ID_BUTTON_3:
                msg.exec = GUI_AooDispChooseFile;
                break;
            case ID_BUTTON_4:
                msg.exec = GUI_ExecCalibrationDialog;
                break;      
            case ID_BUTTON_5:
                msg.exec = GUI_CreateTaskerDialog;
                break;
            default:
                break;    
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
  // // h = GUI_MEMDEV_Create(0, 0, 50, 50);
  //  GUI_MEMDEV_Select(h);
 //   
    //WM_SetCallback(WM_HBKWIN, _cb);
    LCD_SetSizeEx(0, 240, 320);
	LCD_SetVSizeEx(0, 400, 320);
    WM_MOTION_Enable(1);
    
    
    hWin = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog2, WM_HBKWIN, -100, -2);
    GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin, 0, 0);
}



