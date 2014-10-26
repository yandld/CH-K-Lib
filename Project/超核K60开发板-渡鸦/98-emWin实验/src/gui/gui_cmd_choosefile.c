
#include <rtthread.h>
#include "finsh.h"
#include "DIALOG.h"

static rt_thread_t tid1 = RT_NULL;
const char *MYGUI_ExecDialog_ChFile(WM_HWIN hParent, const char *pMask);

static void thread_entry(void* parameter)
{
    
    THREAD_Notepad(WM_HBKWIN, "/MAIN.C");
    #if 0
    int r;
    WM_HWIN  hWin;
    const char *p;
    p = MYGUI_ExecDialog_ChFile(WM_HBKWIN, "*.*");
    if(p != NULL)
    {
        rt_kprintf("FileName:%s\r\n", p);
        return;
    }
    rt_kprintf("NoFile\r\n");
    #endif
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


