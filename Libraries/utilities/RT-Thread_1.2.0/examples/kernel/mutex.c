/*
 * 程序清单：
 */
#include <rtthread.h>
#include "tc_comm.h"

/* 指向线程控制块的指针 */
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_mutex_t mutex = RT_NULL;

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
    rt_err_t result;
    while (1)
    {
        result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            rt_kprintf("thread1: mutex take succ value:%d\r\n", mutex->hold);
            /* 释放互斥锁 */
            rt_thread_delay(100);
            rt_mutex_release(mutex);
        }
        else
        {
            rt_kprintf("thread1: mutex get failed:%d\r\n", result);
        }
    }
}

/* 线程2入口 */
static void thread2_entry(void* parameter)
{
    rt_err_t result;
    rt_thread_delay(5);
    while (1)
    {
        result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            rt_kprintf("thread2: mutex take succ value:%d\r\n", mutex->hold);
            /* 释放互斥锁 */
            rt_mutex_release(mutex);
        }
        else
        {
            rt_kprintf("thread2: mutex take failed:%d\r\n", result);
        }
    }
}


int mutex_simple_init()
{
    /* 创建互斥锁 */
    mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
    if (mutex == RT_NULL)
    {
        rt_kprintf("mutex create failed\r\n");
        return 0;
    }

    /* 创建线程1 */
    tid1 = rt_thread_create("t1",
                            thread1_entry, RT_NULL, /* 线程入口是thread1_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY, 5);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    /* 创建线程2 */
    tid2 = rt_thread_create("t2",
                            thread2_entry, RT_NULL, /* 线程入口是thread2_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY + 1, 5);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);



    return 0;
}

int _tc_mutex_simple()
{
    /* 设置TestCase清理回调函数 */
    mutex_simple_init();

    /* 返回TestCase运行的最长时间 */
    return 100;
}
/* 输出函数命令到finsh shell中 */

FINSH_FUNCTION_EXPORT_ALIAS(_tc_mutex_simple, __cmd_thread_mutex, sime mutex example.);
