#include <rtthread.h>
#include <rthw.h>

#include "DIALOG.h"


static void GUI_SettingInit(void)
{
	GUI_SetBkColor(GUI_BLACK); 	
	GUI_SetColor(GUI_BLACK);
    ICONVIEW_EnableStreamAuto();
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    
}


void gui_thread_entry(void* parameter)
{
    rt_thread_t thread = rt_thread_self();
    GUI_Init();//³õÊ¼»¯GUI 
    GUI_SettingInit();
	GUI_DispString("GUI system OK");//ÏÔÊ¾²âÊÔ
   // CreateWelcome();
  //  GUI_Exec();
  //  show_pic("/SD/PIC/INDEX.BMP");
  //  GUI_CURSOR_Show();
    
    GUIDEMO_IconView();
   // MainMenu_Init();
  //  while(WM_GetWindowOrgY(hWin) > 0)
    {
    //    WM_MoveWindow(hWin, 0, -6);
   //     rt_thread_delay(1);
    }
  //  MainTask();
	while(1)
	{
        
     //   WM_MoveCtrl();
        
       // GUI_TOUCH_Exec();
        rt_thread_delay(10);
	}
}