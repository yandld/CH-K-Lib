#include <rtthread.h>
#include <rthw.h>
#include <components.h>
#include <stdint.h>

void DelayMs(uint32_t ms)
{
    if(ms < (1000/RT_TICK_PER_SECOND)) rt_thread_delay(1);
    else rt_thread_delay(ms/(1000/RT_TICK_PER_SECOND));  
}

void SysTick_Handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

__weak void init_thread_entry(void* parameter)
{
    rt_kprintf("%s is empty!\r\n", __func__);
}


void _init_entry(void* parameter)
{
    rt_thread_t tid;
    
    /* init components */
    rt_components_init();
    dfs_romfs_init();
    elm_init();
    cpu_usage_init();
    finsh_system_init();
    dfs_mount(RT_NULL, "/", "rom", 0, &romfs_root);
    
    /* init thread */
    tid = rt_thread_create("init", init_thread_entry, RT_NULL, 2048, 0x20, 20);                       
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);		
    }
    
    
    tid = rt_thread_self();
    rt_thread_delete(tid);
}


void rt_application_init(void)
{
    rt_thread_t tid;
    
    /* internal init thread */
    tid = rt_thread_create("init0", _init_entry, RT_NULL, 512, 5, 20);                       
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);		
    }
}








