#include "gui_appdef.h"


#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x03)
#define ID_BUTTON_3 (GUI_ID_USER + 0x04)
#define ID_BUTTON_4 (GUI_ID_USER + 0x05)
#define ID_BUTTON_5 (GUI_ID_USER + 0x06)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin",    ID_FRAMEWIN_0, 0, -1, 240, 320, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Cal",       ID_BUTTON_0, 71, 8, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Info",          ID_BUTTON_1, 6, 8, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Time",          ID_BUTTON_2, 137, 8, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "File",       ID_BUTTON_3, 6, 49, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Cali",          ID_BUTTON_4, 71, 49, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Test",          ID_BUTTON_5, 137, 49, 60, 35, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    gui_msg_t msg;
    msg.exec = RT_NULL;
    
    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            hItem = pMsg->hWin;
            FRAMEWIN_SetTitleVis(hItem, 0);
            break;
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
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
                break;
            default:
                break;    
        }
        if(msg.exec != RT_NULL)
        {
            rt_mq_send(guimq, &msg, sizeof(msg));
        }
    }

  default:
    WM_DefaultProc(pMsg);
    break;
  }
}



WM_HWIN gcd(void)
{
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}


FINSH_FUNCTION_EXPORT(gcd, create a directory);


