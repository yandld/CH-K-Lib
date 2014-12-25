#include <rtthread.h>
#include <finsh.h>

#include "gui_appdef.h"


int cmd_ui_test(int argc, char** argv)
{
    rt_thread_t tid = rt_thread_find("gui_exe");
    if(!tid)
    {
        rt_kprintf("ERROR! no GUI\r\n");
        return -1;
    }
    gui_msg_t msg;
    msg.cmd = 2;
    msg.exec = GUI_AppDispCalculator;
    //rt_mq_send(guimq, &msg, sizeof(msg));
    volatile rt_uint32_t i;
    for(i=0;i<1000000;i++)
    {
        printf("");
    }
    rt_malloc(1024*2);
    return 0;
}


FINSH_FUNCTION_EXPORT_ALIAS(cmd_ui_test, __cmd_ui_test, test  test.);
