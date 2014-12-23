#include "gui_appdef.h"
#include "tasker.h"


const GUI_WIDGET_CREATE_INFO _aDialogCreate1[] = {
  { WINDOW_CreateIndirect,    "Dialog 1", 0,                   0,   0, 260, 100, FRAMEWIN_CF_MOVEABLE },
  { BUTTON_CreateIndirect,    "Button",   GUI_ID_BUTTON0,      5,  30,  80,  20, 0},
  { TEXT_CreateIndirect,      "Dialog 1", 0,                   5,  10,  50,  20, TEXT_CF_LEFT }
};


WM_HWIN _TaskerAddPageTest(void)
{
    return GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), NULL, WM_UNATTACHED, 0, 0);
}

