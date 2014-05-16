
#include <rtthread.h>
#include "finsh.h"
#include "DIALOG.h"

static rt_thread_t tid1 = RT_NULL;

static void thread_entry(void* parameter)
{
    WM_HWIN  hWin;
    int r;
    hWin = GUI_Desktop();
    hWin = CreateChooseFile();
   // r = GUI_ExecCreatedDialog(hWin);
   // rt_kprintf("r:%d\r\n", r);
    while (1)
    {
        //GUI_DispString("Hello");
        rt_thread_delay(10);
    }
}

static int _tc_thread_choose_file()
{
    tid1 = rt_thread_create("t1",
        thread_entry, (void*)1,
        4096, 20, 5);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(_tc_thread_choose_file, __cmd_thread_choosefile,  a dynamic thread example);


