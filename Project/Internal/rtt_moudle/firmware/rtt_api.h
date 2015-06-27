#ifndef __RT_API_H__
#define __RT_API_H__

#include <stdint.h>
#include <rtthread.h>


typedef struct
{
    int (*finsh_system_init)(void);
    void (*finsh_set_device)(const char* device_name);
    const char* (*finsh_get_device)(void);
}shell_t;

typedef struct
{
    void (*rt_kprintf)(const char *fmt, ...);
    rt_int32_t (*rt_vsprintf)(char *dest, const char *format, va_list arg_ptr);
    rt_int32_t (*rt_vsnprintf)(char *buf, rt_size_t size, const char *fmt, va_list args);
    rt_int32_t (*rt_sprintf)(char *buf ,const char *format, ...);
    rt_int32_t (*rt_snprintf)(char *buf, rt_size_t size, const char *format, ...);
    rt_device_t (*rt_console_set_device)(const char *name);
    rt_device_t (*rt_console_get_device)(void);
    void * (*rt_malloc)(rt_size_t nbytes);
    void (*rt_free)(void *ptr);
    rt_err_t (*rt_thread_delay)(rt_tick_t tick);
    rt_tick_t (*rt_tick_get)(void);
    rt_device_t (*rt_device_find)(const char *name);
    rt_err_t  (*rt_device_init) (rt_device_t dev);
    rt_thread_t (*rt_thread_create)(const char *name, void (*entry)(void *parameter), void *parameter, rt_uint32_t stack_size, rt_uint8_t  priority,rt_uint32_t tick);
    rt_err_t (*rt_thread_startup)(rt_thread_t thread);
}rtthread_t;

typedef struct
{
    const shell_t * shell;
    const rtthread_t *rtthread;
    
}api_t;

void API_SetAddr(uint32_t addr);










#endif



