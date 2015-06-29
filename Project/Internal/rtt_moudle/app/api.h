#ifndef __API_H__
#define __API_H__

#include "rtt_api.h"

extern api_t *api;

#define rt_pin_mode             api->pin->rt_pin_mode
#define rt_pin_write            api->pin->rt_pin_write
#define rt_pin_read             api->pin->rt_pin_read

#define rt_kprintf              api->rtthread->rt_kprintf
#define rt_thread_delay         api->rtthread->rt_thread_delay
#define rt_malloc               api->rtthread->rt_malloc
#define rt_tick_get             api->rtthread->rt_tick_get
#define rt_device_find          api->rtthread->rt_device_find
#define rt_thread_create        api->rtthread->rt_thread_create
#define rt_device_register      api->rtthread->rt_device_register
#define rt_thread_startup       api->rtthread->rt_thread_startup
#define rt_device_init          api->rtthread->rt_device_init
#define rt_device_control       api->rtthread->rt_device_control
#define rt_device_open          api->rtthread->rt_device_open
#define rt_device_read          api->rtthread->rt_device_read
#define rt_device_write         api->rtthread->rt_device_write
#define rt_thread_find          api->rtthread->rt_thread_find
#define finsh_syscall_append    api->finsh->finsh_syscall_append


#endif





