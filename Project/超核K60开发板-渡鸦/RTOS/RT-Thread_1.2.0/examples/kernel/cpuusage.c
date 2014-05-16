#include <rtthread.h>
#include <rthw.h>
#include "finsh.h"

#define CPU_USAGE_CALC_TICK    10
#define CPU_USAGE_LOOP        100

static rt_uint8_t  cpu_usage_major = 0, cpu_usage_minor= 0;
static rt_uint32_t total_count = 0;

static void cpu_usage_idle_hook()
{
    rt_tick_t tick;
    rt_uint32_t count;
    volatile rt_uint32_t loop;

    if (total_count == 0)
    {
        /* get total count */
        rt_enter_critical();
        tick = rt_tick_get();
        while(rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
        {
            total_count ++;
            loop = 0;
            while (loop < CPU_USAGE_LOOP) loop ++;
        }
        rt_exit_critical();
    }

    count = 0;
    /* get CPU usage */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
        count ++;
        loop  = 0;
        while (loop < CPU_USAGE_LOOP) loop ++;
    }

    /* calculate major and minor */
    if (count < total_count)
    {
        count = total_count - count;
        cpu_usage_major = (count * 100) / total_count;
        cpu_usage_minor = ((count * 100) % total_count) * 100 / total_count;
    }
    else
    {
        total_count = count;

        /* no CPU usage */
        cpu_usage_major = 0;
        cpu_usage_minor = 0;
    }
}

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor)
{
    RT_ASSERT(major != RT_NULL);
    RT_ASSERT(minor != RT_NULL);

    *major = cpu_usage_major;
    *minor = cpu_usage_minor;
}

void cpu_usage_init()
{
    /* set idle thread hook */
    rt_thread_idle_sethook(cpu_usage_idle_hook);
}

int _cmd_cpu_usage()
{
    rt_uint8_t major, minor;
    cpu_usage_init();
    cpu_usage_get(&major, &minor);
    rt_kprintf("CPUUsage: %d.%d%\r\n", major, minor);
    return 30;
}

int _cmd_showmem(int argc, char** argv)
{
    if(argc == 3)
    {
        rt_hw_show_memory(strtoul(argv[1],0,0), strtoul(argv[2],0,0));
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(_cmd_showmem, __cmd_showmem, show mem eg:showmem 0x2000000 200.);
FINSH_FUNCTION_EXPORT_ALIAS(_cmd_cpu_usage, __cmd_cpuusage, example delay.);

