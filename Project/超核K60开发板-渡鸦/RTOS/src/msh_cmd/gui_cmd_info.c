#include <rtthread.h>
#include "common.h"
#include "GUI.H"

#ifdef FINSH_USING_MSH
#include "finsh.h"

int sys_info(int argc, char** argv)
{
    rt_uint32_t clock, tick;
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    rt_kprintf("CoreClock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    rt_kprintf("CoreClock:%dHz\r\n", clock);
    
    tick = rt_tick_get();
    rt_kprintf("tick:%d | time:%ds\r\n", tick, tick/RT_TICK_PER_SECOND);
    
    rt_kprintf("ui_mem_total:%d\r\n", GUI_ALLOC_GetNumUsedBytes()+GUI_ALLOC_GetNumFreeBytes());
    rt_kprintf("ui_mem_free:%d\r\n", GUI_ALLOC_GetNumFreeBytes());
    
    return 0;
}


MSH_CMD_EXPORT(sys_info, show a picture file.);
#endif





