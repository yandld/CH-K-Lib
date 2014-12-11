
#include "DIALOG.h"

#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_MULTIPAGE_0  (GUI_ID_USER + 0x04)
#define ID_LISTVIEW_0  (GUI_ID_USER + 0x05)
#define ID_BUTTON_0  (GUI_ID_USER + 0x06)


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Task Manager", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_0, 0, 2, 232, 306, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 8, 26, 219, 254, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 8, 281, 70, 17, 0, 0x0, 0 },

};

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;


  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Task Manager'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetFont(hItem, GUI_FONT_13_1);
    //
    // Initialization of 'Multipage'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_0);
    MULTIPAGE_AddEmptyPage(hItem, 0, "Tasks");
    MULTIPAGE_AddEmptyPage(hItem, 0, "Performonce");
    MULTIPAGE_AddEmptyPage(hItem, 0, "Networking");
    MULTIPAGE_AddEmptyPage(hItem, 0, "About");
    //
    // Initialization of 'Listview'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    LISTVIEW_AddColumn(hItem, 80, "Thread", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 50, "Priority", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 60, "Stacksize", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_SetAutoScrollV(hItem, 1);
    LISTVIEW_SetItemText(hItem, 0, 0, "Item");
    LISTVIEW_SetAutoScrollH(hItem, 1);
    LISTVIEW_SetGridVis(hItem, 1);
    LISTVIEW_AddColumn(hItem, 30, "Col", GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetText(hItem, "End Task");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_MULTIPAGE_0: // Notifications sent by 'Multipage'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTVIEW_0: // Notifications sent by 'Listview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_0: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
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



WM_HWIN GUI_AppDispTaskManager(void)
{
  WM_HWIN hWin;

  hWin = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}
