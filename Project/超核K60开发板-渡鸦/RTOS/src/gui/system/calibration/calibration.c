#include "gui_appdef.h"


#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_TEXT_0  (GUI_ID_USER + 0x01)
#define ID_TEXT_1  (GUI_ID_USER + 0x02)
#define ID_BUTTON_0  (GUI_ID_USER + 0x04)
#define ID_BUTTON_1  (GUI_ID_USER + 0x05)
#define ID_TEXT_2  (GUI_ID_USER + 0x06)
#define ID_TEXT_3  (GUI_ID_USER + 0x07)

  int aPhysX[2], aPhysY[2], aLogX[2], aLogY[2], i;
  
  
  
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 18, 104, 194, 109, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 67, 0, 95, 17, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 0, 0, 13, 13, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 215, 300, 13, 13, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "upper left position", ID_TEXT_2, 2, 14, 98, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "lower right position", ID_TEXT_3, 137, 283, 94, 20, 0, 0x64, 0 },
};

static void _DispStringCentered(const char * pString) {
  GUI_RECT Rect;

  Rect.x0 = Rect.y0 = 0;
  Rect.x1 = LCD_GetXSize() - 1;
  Rect.y1 = LCD_GetYSize() - 1;
  GUI_DispStringInRect(pString, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}


static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            hItem = pMsg->hWin;
            FRAMEWIN_SetTitleVis(hItem, 0);
            GUI_SetFont(&GUI_Font13B_ASCII);
            // 
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
            TEXT_SetWrapMode(hItem, GUI_WRAPMODE_CHAR);
            TEXT_SetTextAlign(hItem, GUI_TA_HORIZONTAL|GUI_TA_VCENTER);
            TEXT_SetText(hItem,"This sample shows how a touch screen can be calibrated at run time.\n"
                               "Please press the touch screen to continue...");
            //
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
            TEXT_SetText(hItem, "TOUCH_Calibrate");
            //
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
            BUTTON_SetText(hItem, " ");
            WM_HideWindow(hItem);
        break;

        default:
            WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN GUI_ExecCalibrationDialog(void* parameter)
{
    WM_HWIN hWin;
    hWin = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

