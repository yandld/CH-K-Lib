#include <rtthread.h>
#include <rthw.h>

  #include "WM.h"

  #include "CHECKBOX.h"
  #include "TREEVIEW.h"




void gui_thread_entry(void* parameter)
{
    rt_thread_t thread = rt_thread_self();
    GUI_Init();//≥ı ºªØGUI 
	GUI_DispString("GUI system OK");//œ‘ æ≤‚ ‘
   // CreateWelcome();
  //  GUI_Exec();
  //  show_pic("/SD/PIC/INDEX.BMP");
  //  GUI_CURSOR_Show();
    
    CreateRaven();
  //  MainTask();
	while(1)
	{
       // GUI_TOUCH_Exec();
        rt_thread_delay(10);
	}
}