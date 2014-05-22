
#include "DIALOG.h"
#include "gui_image.h"


#define ID_FRAMEWIN_0        (GUI_ID_USER + 0x00)
#define ID_BUTTON_0        (GUI_ID_USER + 0x01)
#define ID_BUTTON_1        (GUI_ID_USER + 0x02)
#define ID_BUTTON_2        (GUI_ID_USER + 0x03)
#define ID_BUTTON_3        (GUI_ID_USER + 0x04)


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Desktop", ID_FRAMEWIN_0, 0, -1, 240, 320, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Cal", ID_BUTTON_0, 5, 9, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "CF", ID_BUTTON_1, 71, 9, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Time", ID_BUTTON_2, 140, 9, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "NetWork", ID_BUTTON_3, 5, 50, 60, 35, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

char * MYGUI_DLG_ChFileGetPath(WM_HWIN hItem);

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

    rt_kprintf("pMsg:%d\r\n", pMsg->MsgId);
    
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = pMsg->hWin;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
          MYGUI_DLG_Calender(WM_HBKWIN);
        break;
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
          MYGUI_DLG_ChFile(WM_HBKWIN);
        break;
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
          MYGUI_DLG_Time(WM_HBKWIN);
        break;
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}


#if 0
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN    hItem;
    WM_HWIN    hDlg;
    int        NCode;
    int        Sel;
    int        Id;
    hDlg = pMsg->hWin;
    switch (pMsg->MsgId)
    {
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
            NCode = pMsg->Data.v;                 /* Notification code */
            switch (Id)
            {
                case GUI_ID_ICONVIEW0:
                    switch (NCode)
                    {
                        case WM_NOTIFICATION_SEL_CHANGED:
                        switch(Sel)
                        {
                            case 2:
                            MYGUI_DLG_Calender(pMsg->hWinSrc);
                                break;
                            case 1:
                           // GUI_X_Delay(10);
                            CreateClock();
                                break;
                            case 3:
                                MYGUI_DLG_ChooseFile(pMsg->hWinSrc);
                               // CreateChooseFile(NULL);
                                break;
                            case 4:
                                GUI_IMAGE_CreateWidget(pMsg->hWinSrc);
                                WM_HideWindow(pMsg->hWinSrc);
                                break;
                        }
                        //WM_InvalidateWindow(pMsg->hWinSrc);
                        
                        //ICONVIEW_SetSel(pMsg->hWinSrc, 0);
                        Sel = 255;
                        break;
                    }
                    break;
            }
            break;

        case WM_PAINT:
            GUI_SetBkColor(GUI_BLACK);
            GUI_Clear();
            break;
        default:
            WM_DefaultProc(pMsg);
    }
}
#endif

WM_HWIN MYGUI_DLG_CreateDesktop(void)
{
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    while(1)
    {
        static char *p;
        while(p == NULL)
        {
            p = MYGUI_DLG_ChFileGetPath(WM_HBKWIN);
            rt_thread_delay(10);
        }
        rt_kprintf("%s\r\n", p);
        p = NULL;
    }
    rt_thread_delay(5);
    return hWin;
}


