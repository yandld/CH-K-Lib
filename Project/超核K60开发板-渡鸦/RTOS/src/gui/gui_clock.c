#include "DIALOG.h"
#include <stdint.h>
#include <time.h>


#define ID_FRAMEWIN_0       (GUI_ID_USER + 0x00)
#define ID_BUTTON_0         (GUI_ID_USER + 0x03)
#define ID_BUTTON_1         (GUI_ID_USER + 0x04)
#define ID_TEXT_0           (GUI_ID_USER + 0x05)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
  { FRAMEWIN_CreateIndirect, "RTC Clock", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Sun", ID_TEXT_0, 5, 4, 165, 23, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "OK", ID_BUTTON_0, 0, 27, 60, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Cancel", ID_BUTTON_1, 119, 27, 60, 20, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  time_t now;
    switch (pMsg->MsgId)
    {
        case WM_TIMER:
            WM_RestartTimer(pMsg->Data.v, 1000);
            now = time(0);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetText(hItem, ctime(&now));
            break;
        case WM_INIT_DIALOG:
            hItem = pMsg->hWin;
            FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
            WM_MakeModal(hItem);
            WM_CreateTimer(WM_GetClientWindow(pMsg->hWin), 0, 1000, 0);
        
            /* init text */
            now = time(0);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetFont(hItem, GUI_FONT_16B_1);
            TEXT_SetTextColor(hItem, 0x000000FF);
            TEXT_SetText(hItem, ctime(&now));
            break;
        case WM_NOTIFY_PARENT:
            Id    = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id)
            {
                case ID_BUTTON_0: // Notifications sent by 'OK'

                switch(NCode)
                {
                    case WM_NOTIFICATION_LOST_FOCUS:
                    case WM_NOTIFICATION_CLICKED:
                        GUI_EndDialog(pMsg->hWin, 0);
                        break;
                }
                break;
                case ID_BUTTON_1: // Notifications sent by 'Back'
                switch(NCode)
                {
                    case WM_NOTIFICATION_CLICKED:
                        GUI_EndDialog(pMsg->hWin, 0);
                        break;
                }
                break;
            }
        break;
    default:
    WM_DefaultProc(pMsg);
    break;
    }
}

void GUI_AppDispTime(void)
{
    GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
}

