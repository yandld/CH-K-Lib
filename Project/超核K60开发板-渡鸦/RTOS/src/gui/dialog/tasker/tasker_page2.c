#include "gui_appdef.h"


#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0     (GUI_ID_USER + 0x05)


const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
  { WINDOW_CreateIndirect,    "Dialog 1", 0,                   0,   0, 250, 270, FRAMEWIN_CF_MOVEABLE },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,        6,  10, 210, 240, 0, 0x0, 0 },

};

