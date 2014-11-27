
#include "DIALOG.h"
#include "gui_image.h"
#include <finsh.h>

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x03)
#define ID_BUTTON_3 (GUI_ID_USER + 0x04)
#define ID_BUTTON_4 (GUI_ID_USER + 0x05)


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, -1, 240, 320, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "CF", ID_BUTTON_0, 71, 8, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Cal", ID_BUTTON_1, 6, 8, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Time", ID_BUTTON_2, 137, 8, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "NetWork", ID_BUTTON_3, 6, 49, 60, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Reader", ID_BUTTON_4, 71, 49, 60, 35, 0, 0x0, 0 },
};


static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

    //rt_kprintf("pMsg:%d\r\n", pMsg->MsgId);
    
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = pMsg->hWin;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0:
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
          MYGUI_DLG_Calender(WM_HBKWIN);
        break;
      }
      break;
    case ID_BUTTON_1:
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
      //    MYGUI_DLG_ChFile(WM_HBKWIN);
        break;
      }
      break;
    case ID_BUTTON_2:
      switch(NCode) {

      case WM_NOTIFICATION_RELEASED:
          MYGUI_DLG_Time(WM_HBKWIN);
        break;
      }
      break;
    case ID_BUTTON_4: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
          THREAD_Notepad(WM_HBKWIN, "/MAIN.C");
            
        break;
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;

  default:
    WM_DefaultProc(pMsg);
    break;
  }
}



WM_HWIN gcd(void)
{
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
//    while(1)
//    {
//        rt_thread_delay(10);
//    }
    return hWin;
}


FINSH_FUNCTION_EXPORT(gcd, create a directory);


