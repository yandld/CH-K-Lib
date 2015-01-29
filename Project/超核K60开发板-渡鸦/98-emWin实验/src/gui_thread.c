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
    
    GUI_Init();
    GUI_SettingInit();
    GUI_DispString("GUI system OK\r\n");//œ‘ æ≤‚ ‘
    GUI_DispString(GUI_GetVersionString());
    
    MainTask();
	while(1)
	{
        rt_thread_delay(10);
	}
}
