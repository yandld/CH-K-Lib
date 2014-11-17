#include <rtthread.h>
#include <rthw.h>
#include "finsh.h"



int run_time(int argc, char ** argv)
{
    rt_uint32_t tick;
    tick = rt_tick_get();
    rt_kprintf("tick:%d | time:%ds\r\n", tick, tick/RT_TICK_PER_SECOND);
    return 0;
}

MSH_CMD_EXPORT(run_time, get system time);



