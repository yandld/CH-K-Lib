
#include "DIALOG.h"
#include "gui_image.h"

typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
  const char       * pExplanation;
} BITMAP_ITEM;

GUI_CONST_STORAGE GUI_BITMAP _bmRead = {
  48, /* XSize */
  48, /* YSize */
  192, /* BytesPerLine */
  32, /* BitsPerPixel */
  (unsigned char *)NULL,  /* Pointer to picture data */
  NULL  /* Pointer to palette */
 ,GUI_DRAW_BMP8888
};



static const BITMAP_ITEM _aBitmapItem[] = {
  {&_bmRead, "Browser",  "Use the browser to explore the www"},
  {&_bmRead,   "Clock",    "Adjust current time and date"},
  {&_bmRead,    "Date",     "Use the diary"},
  {&_bmRead,   "Email",    "Read an email"},
  {&_bmRead,    "Picture",     "Read a document"},
};


static void _cbBk(WM_MESSAGE * pMsg) {
  GUI_RECT   Rect       = { 0, 0, 0, 0 };
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
                        Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
                        hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT1);
                        TEXT_SetText(hItem, _aBitmapItem[Sel].pExplanation);
                        switch(Sel)
                        {
                            case 2:
                            GUI_X_Delay(10);
                            CreateCalendar();
                                break;
                            case 1:
                           // GUI_X_Delay(10);
                            CreateClock();
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



WM_HWIN GUI_Desktop(void)
{
    WM_HWIN       hWin;
    int           xSize;
    U32           i;

    // GUIDEMO_ShowIntro("Icon View Demo", "Demonstrates the use of\nthe ICONVIEW widget");
    // GUIDEMO_HideInfoWin();
    WM_EnableMemdev(WM_HBKWIN);
    WM_SetCallback(WM_HBKWIN, _cbBk);

    // _ScreenX0 = (xSize - XSIZE_MIN) / 2; 
    //  _ScreenY0 = (ySize - YSIZE_MIN) / 2; 
    //
    // Create title of sample
    //
     //hTextTitle = TEXT_CreateEx(_ScreenX0, (ySize - _bmDog.YSize) / 2 - TITLE_HEIGHT, XSIZE_MIN, TITLE_HEIGHT, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "emWin ICONVIEW sample");
    // TEXT_SetFont(hTextTitle, &GUI_FontRounded22);
    //  TEXT_SetTextAlign(hTextTitle, GUI_TA_HCENTER | GUI_TA_VCENTER);
    // TEXT_SetTextColor(hTextTitle, TITLE_COLOR);
    //
    // Create explanation
    //
     // hTextExpl = TEXT_CreateEx(xSize / 2, (ySize - XSIZE_MIN) / 2 + 40, 140, 100, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT1, "");
    //  GUI_SIF_CreateFont(_acFramed24B, &Font, GUI_SIF_TYPE_PROP_FRM);
    //  TEXT_SetFont(hTextExpl, &Font);
    //  TEXT_SetTextColor(hTextExpl, GUI_WHITE);
    //  TEXT_SetWrapMode(hTextExpl, GUI_WRAPMODE_WORD);
    // TEXT_SetTextAlign(hTextExpl, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Create ICONVIEW widget
    //
    hWin = ICONVIEW_CreateEx(0,0, 240, 320, WM_HBKWIN, WM_CF_SHOW | WM_CF_HASTRANS, ICONVIEW_CF_AUTOSCROLLBAR_V, GUI_ID_ICONVIEW0, 55, 60);
    for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++)
    {
        ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[i].pBitmap, _aBitmapItem[i].pText);
    }
    ICONVIEW_SetBkColor(hWin, ICONVIEW_CI_SEL, GUI_BLUE | 0xC0000000);
    ICONVIEW_SetFont(hWin, &GUI_Font13B_ASCII);
    WM_SetFocus(hWin);
    return hWin;
}

