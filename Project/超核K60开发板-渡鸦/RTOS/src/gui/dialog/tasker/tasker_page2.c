#include "gui_appdef.h"


#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0     (GUI_ID_USER + 0x05)


const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
  { WINDOW_CreateIndirect,    "Dialog 1", 0,                   0,   0, 250, 270, FRAMEWIN_CF_MOVEABLE },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,        6,  10, 210, 240, 0, 0x0, 0 },

};

void _cbDialog(WM_MESSAGE * pMsg)
{
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
    //
    // Initialization of 'Listview'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    LISTVIEW_AddColumn(hItem, 50, "Thread", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 40, "Prioity", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 40, "Stack", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_SetGridVis(hItem, 1);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

WM_HWIN _TaskerAddPageThread(void)
{
    return GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog, WM_UNATTACHED, 0, 0);
}

