#include <rtthread.h>
#include <stdint.h>
#include "api.h"
#include "gui.h"
//#include "gui_appdef.h"

//rt_mq_t guimq;

void gui_thread_entry(void* parameter)
{
    //guimq = rt_mq_create("gui_mq", sizeof(gui_msg_t), 6, RT_IPC_FLAG_FIFO);
    //gui_msg_t msg;
    
    GUI_Init();
    GUI_DispString("gui system actived!\r\n");

   // GUI_CURSOR_Show();
   // mkdir("/SF/SYS", 0);
   // GUI_AppAutoCalibration("/SF/SYS/TDATA");
  //  GUI_CreateDesktopDialog();
	while(1)
	{
//        if(rt_mq_recv(guimq, &msg, sizeof(gui_msg_t), 1) == RT_EOK)
//        {
//            if(msg.exec != RT_NULL)
//            {
//                msg.exec(msg.parameter);
//            } 
//        }
        rt_thread_delay(1);
     //   GUI_Exec();
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


int ui_startup(int argc, char** argv)
{
    int ret;
    rt_thread_t tid;
    rt_device_t lcd;

    tid = rt_thread_find("gui");
    if(tid != RT_NULL)
        return RT_ERROR;
    
    lcd = rt_device_find("lcd0");
    if(!lcd)
        return RT_ERROR;

    ret = rt_device_init(lcd);
    if(ret)
        return RT_ERROR;

    
    
    rt_kprintf("GUI system start!\r\n");
    tid = rt_thread_create("gui", gui_thread_entry, RT_NULL, (1024*2), 0x13, 20);                                
    rt_thread_startup(tid);

    tid = rt_thread_create("guit", guit_thread_entry, RT_NULL, (512), 0x14, 20);                                                      
    rt_thread_startup(tid);

    return RT_EOK;
}


