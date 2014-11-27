#include <rtthread.h>
#include <finsh.h>
#include "GUI.H"

void gui_thread_entry(void* parameter)
{
	while(1)
	{
        GUI_Exec();
	}
}

void guit_thread_entry(void* parameter)
{
	while(1)
	{
        GUI_TOUCH_Exec();
        rt_thread_delay(1);
	}
}


int cmd_gui_start(int argc, char** argv)
{
    rt_thread_t tid;
    
    /* this task can not be single */
    tid = rt_thread_find("gui_exe");
    if(tid != RT_NULL) return -1;
    
    /* init GUI system */
    GUI_Init();
    
    /* create gui thread and run */
    tid = rt_thread_create("gui_exe", gui_thread_entry, RT_NULL, (1024*50), 0x27, 20);                                                      
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);		
    }
    
    tid = rt_thread_create("guit_exe", guit_thread_entry, RT_NULL, (1024*1), 0x23, 20);                                                      
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);		
    }
    
    GUI_DispString("gui system actived!\r\n");
    GUI_CURSOR_Show();
    gcd();
    //TOUCH_MainTask();
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_gui_start, __cmd_gui_start, start gui system.);

