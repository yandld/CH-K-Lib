#include <rtthread.h>
#include <finsh.h>
#include "GUI.H"

void gui_demo_thread_entry(void* parameter)
{
    rt_kprintf("\r\ngui demo task\r\n");
    
	while(1)
	{
      //  GUI_TOUCH_Exec();
        GUI_Exec();
        rt_thread_delay(1);
	}
}

void gui_demo_main_thread_entry(void* parameter)
{
    // MainTask();
    GUI_Init();
    // MYGUI_DLG_CreateDesktop();
}


int cmd_gui_demo(int argc, char** argv)
{
    rt_thread_t thread;
    
    /* this task can not be single */
    thread = rt_thread_find("gui_demo");
    if(thread != RT_NULL)
    {
        rt_kprintf("task:gui_demo already existed\r\n");
        return 1;
    }
    
    /* create gui thread and run */
    thread = rt_thread_create("gui", gui_demo_thread_entry, RT_NULL, (1024*50), 0x27, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    thread = rt_thread_create("gui2", gui_demo_main_thread_entry, RT_NULL, (1024*2), 0x24, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    //TOUCH_MainTask();
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_gui_demo, __cmd_gui_demo, run emwin demo.);

