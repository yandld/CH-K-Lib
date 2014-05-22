#include <rtthread.h>
#include <rthw.h>
#include <stdint.h>
#include "DIALOG.h"
#include <math.h>


static void GUI_SettingInit(void)
{
	GUI_SetBkColor(GUI_BLACK); 	
	GUI_SetColor(GUI_WHITE);
    ICONVIEW_EnableStreamAuto();
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
}

void gui_thread_entry(void* parameter)
{
    rt_thread_t thread = rt_thread_self();
    rt_kprintf("gui_thread_entry\r\n");
    GUI_Init();
    GUI_SettingInit();
	GUI_DispString("GUI system OK");//œ‘ æ≤‚ ‘

    WM_HWIN hWin;
  //  GUI_CURSOR_Show();
    hWin = MYGUI_DLG_CreateDesktop();

   // MainMenu_Init();

	while(1)
	{
        rt_thread_delay(1);
	}
}
