#include <rtthread.h>
#include <finsh.h>
#include "gui_appdef.h"
#include "GUI.H"
#include "WM.H"

rt_mq_t guimq;

void gui_thread_entry(void* parameter)
{
    U32 i;
    guimq = rt_mq_create("gui_mq", sizeof(gui_msg_t), 6, RT_IPC_FLAG_FIFO);
    gui_msg_t msg;
    GUI_Init();
    GUI_DispString("gui system actived!\r\n");
    GUI_Delay(100);
    TOUCH_MainTask();
    GUI_CURSOR_Show();
    gcd();
	while(1)
	{
        if(rt_mq_recv(guimq, &msg, sizeof(gui_msg_t), 1) == RT_EOK)
        {
            rt_kprintf("%s", msg.parameter);
            msg.exec(msg.parameter);
        }
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
    
    /* create gui thread and run */
    tid = rt_thread_create("gui_exe", gui_thread_entry, RT_NULL, (1024*8), 0x15, 20);                                
    rt_thread_startup(tid);

    tid = rt_thread_create("guit_exe", guit_thread_entry, RT_NULL, (512), 0x14, 20);                                                      
    rt_thread_startup(tid);	

    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_gui_start, __cmd_gui_start, start gui system.);

