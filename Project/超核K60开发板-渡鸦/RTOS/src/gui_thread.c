#include <rtthread.h>
#include <rthw.h>

  #include "WM.h"

  #include "CHECKBOX.h"
  #include "FRAMEWIN.h"
  #include "PROGBAR.h"
  #include "TEXT.h"
  #include "BUTTON.h"
  #include "SLIDER.h"
  #include "HEADER.h"
  #include "GRAPH.h"
  #include "ICONVIEW.h"
  #include "LISTVIEW.h"
  #include "TREEVIEW.h"

void gui_thread_entry(void* parameter)
{
    rt_thread_t thread = rt_thread_self();
    GUI_Init();//≥ı ºªØGUI 
	GUI_DispString("GUI system OK");//œ‘ æ≤‚ ‘
    CreateWelcome();
    GUI_Exec();
  //  show_pic("/SD/PIC/INDEX.BMP");
	while(1)
	{
        rt_thread_delay(50);
	}
}