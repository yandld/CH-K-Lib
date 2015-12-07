#include <rtthread.h>
#include <stdint.h>
#include "gui.h"
#include "gui_appdef.h"

#include "k_module.h"

void gui_thread(void* parameter)
{
//    guimq = rt_mq_create("gui_mq", sizeof(gui_msg_t), 6, RT_IPC_FLAG_FIFO);
//    gui_msg_t msg;

   // k_StartUp();
    k_InitMenu();
    /* Gui background Task */
    while(1)
    {
        GUI_Exec(); /* Do the background work ... Update windows etc.) */
        rt_thread_delay(1); /* Nothing left to do for the moment ... Idle processing */
    }
//    //GUI_CURSOR_Show();
//   // MainTask();
//   // mkdir("/SF/SYS", 0);
//   // GUI_AppAutoCalibration("/SF/SYS/TDATA");
// //  GUI_CreateDesktopDialog();
//	while(1)
//	{
//        if(rt_mq_recv(guimq, &msg, sizeof(gui_msg_t), 1) == RT_EOK)
//        {
//            if(msg.exec != RT_NULL)
//            {
//                msg.exec(msg.parameter);
//            } 
//        }
//        GUI_Exec();
//	}
}

void guit_thread_entry(void* parameter)
{
	while(1)
	{
        GUI_TOUCH_Exec();
        rt_thread_delay(1);
	}
}


int ui_startup(void)
{
    rt_thread_t tid;
    
    k_ModuleInit();
    
    GUI_Init();   
    WM_MULTIBUF_Enable(1);
    GUI_SetLayerVisEx (1, 0);
    GUI_SelectLayer(0);
  
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();  

   /* Set General Graphical proprieties */
    k_SetGuiProfile();
    
    tid = rt_thread_create("gui", gui_thread, RT_NULL, (1024*2), 0x17, 20);                                
    rt_thread_startup(tid);

//    tid = rt_thread_create("guit", guit_thread_entry, RT_NULL, (1024*1), 0x26, 20);                                                      
//    rt_thread_startup(tid);

    return RT_EOK;
}


