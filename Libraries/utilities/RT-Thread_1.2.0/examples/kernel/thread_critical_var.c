#include <rtthread.h>
#include "finsh.h"

static rt_uint32_t gTemp;

static void thread_entry(void* parameter)
{
    rt_uint32_t i;
    gTemp = 0;
    /* comments rt_enter_critical and rt_exit_critical to see result */
    //rt_enter_critical();
    for(i=0;i<10;i++)
    {
        gTemp++;
        rt_thread_delay(1);
    }
    //rt_exit_critical();
    rt_kprintf("g_temp:%d\r\n", gTemp);
    
}

static void thread_entry2(void* parameter)
{
    rt_thread_delay(5);
    gTemp++;
}

int _cmd_thread_critical_var(void)
{
    rt_thread_t thread;
    thread = rt_thread_create("tcv1", thread_entry, RT_NULL, 1024, 30, 5);     
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    thread = rt_thread_create("tcv2", thread_entry2, RT_NULL, 1024, 30, 5);     
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    return 30;
}


FINSH_FUNCTION_EXPORT_ALIAS(_cmd_thread_critical_var, __cmd_thread_critical_var,  thread_critical_var.);



