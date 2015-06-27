#include "rtt_api.h"


api_t *api;

void API_SetAddr(uint32_t addr)
{
    api = (api_t*)(addr);
}


rt_err_t rt_thread_delay(rt_tick_t tick)
{
    return api->rtthread->rt_thread_delay(tick);
}

void *rt_malloc(rt_size_t nbytes)
{
    return api->rtthread->rt_malloc(nbytes);
}

rt_tick_t rt_tick_get(void)
{
    return api->rtthread->rt_tick_get();
}

rt_device_t rt_device_find(const char *name)
{
    return api->rtthread->rt_device_find(name);
}

rt_thread_t rt_thread_create(const char *name,void (*entry)(void *parameter), void *parameter, rt_uint32_t stack_size,rt_uint8_t  priority,rt_uint32_t tick)
{
    return api->rtthread->rt_thread_create(name, entry, parameter, stack_size, priority, tick);
}

rt_err_t rt_thread_startup(rt_thread_t thread)
{
    return api->rtthread->rt_thread_startup(thread);
}

rt_err_t rt_device_init(rt_device_t dev)
{
    return api->rtthread->rt_device_init(dev);
}

#include <stdarg.h>
void rt_kprintf(const char *fmt, ...)
{
    va_list args;
    va_start( args, fmt );
    api->rtthread->rt_kprintf(fmt, args);
    va_end(args);
}
