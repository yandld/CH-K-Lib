
#include "gui_appdef.h"

#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_MULTIPAGE_0  (GUI_ID_USER + 0x04)


extern const GUI_WIDGET_CREATE_INFO _aDialogCreate1[3];
extern const GUI_WIDGET_CREATE_INFO _aDialogCreate2[2];
    

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Task Manager", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_0, 0, 2, 232, 306, 0, 0x0, 0 },

};

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;


  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:

    hItem = pMsg->hWin;
    FRAMEWIN_SetFont(hItem, GUI_FONT_13_1);
    FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_0);

    WM_HWIN hDialog;
    hDialog = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), NULL, WM_UNATTACHED, 0, 0);
    MULTIPAGE_AddPage(hItem, hDialog, "Thread");
    hDialog = GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), NULL, WM_UNATTACHED, 0, 0);
    MULTIPAGE_AddPage(hItem, hDialog, "Page1");

    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_MULTIPAGE_0: // Notifications sent by 'Multipage'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:

        break;
      case WM_NOTIFICATION_RELEASED:

        break;
      case WM_NOTIFICATION_VALUE_CHANGED:

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



WM_HWIN GUI_CreateTaskerDialog(void)
{
  WM_HWIN hWin;

  hWin = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}
