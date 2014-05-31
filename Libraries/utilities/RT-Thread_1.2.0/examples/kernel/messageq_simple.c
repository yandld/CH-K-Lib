/*
 * 程序清单：消息队列例程
 *
 * 这个程序会创建3个动态线程，一个线程会从消息队列中收取消息；一个线程会定时给消
 * 息队列发送消息；一个线程会定时给消息队列发送紧急消息。
 */
#include <rtthread.h>
#include "tc_comm.h"

/* 指向线程控制块的指针 */
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;

/* 消息队列控制块 */
static  rt_mq_t mq;
/* 消息队列中用到的放置消息的内存池 */
//static char msg_pool[2048];

/* 线程1入口函数 */
static void thread1_entry(void* parameter)
{
    char buf[128];

    while (1)
    {
        rt_memset(&buf[0], 0, sizeof(buf));

        /* 从消息队列中接收消息 */
        if (rt_mq_recv(mq, &buf[0], sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: recv msg from message queue, the content:%s\n", buf);
        }

        /* 延迟10个OS Tick */
        rt_thread_delay(30);
    }
}

/* 线程2入口函数 */
static void thread2_entry(void* parameter)
{
    int i, result;
    char buf[] = "this is message No.x";

    while (1)
    {
        for (i = 0; i < 5; i++)
        {
            buf[sizeof(buf) - 2] = '0' + i;

            rt_kprintf("thread2: send message - %s\n", buf);
            /* 发送消息到消息队列中 */
            result = rt_mq_send(mq, &buf[0], sizeof(buf));
            if ( result == -RT_EFULL)
            {
                /* 消息队列满， 延迟1s时间 */
                rt_kprintf("message queue full, delay 4s\n");
                rt_thread_delay(400);
            }
        }
        /* 延时10个OS Tick */
        rt_thread_delay(20);
    }
}

/* 线程3入口函数 */
static void thread3_entry(void* parameter)
{
    char buf[] = "this is an urgent message!";

    while (1)
    {
       // rt_kprintf("thread3: send an urgent message\n");

        /* 发送紧急消息到消息队列中 */
        rt_mq_urgent(mq, &buf[0], sizeof(buf));

        /* 延时25个OS Tick */
        rt_thread_delay(100);
    }
}

int messageq_simple_init()
{
    /* 初始化消息队列 */
    mq = rt_mq_create("mqt", 128-sizeof(void*), 10, RT_IPC_FLAG_FIFO);
    /* 创建线程1 */
    tid1 = rt_thread_create("t1",
                            thread1_entry, RT_NULL, /* 线程入口是thread1_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
    else
        tc_stat(TC_STAT_END | TC_STAT_FAILED);

    /* 创建线程2 */
    tid2 = rt_thread_create("t2",
                            thread2_entry, RT_NULL, /* 线程入口是thread2_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
    else
        tc_stat(TC_STAT_END | TC_STAT_FAILED);

    /* 创建线程3 */
    tid3 = rt_thread_create("t3",
                            thread3_entry, RT_NULL, /* 线程入口是thread3_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid3 != RT_NULL)
        rt_thread_startup(tid3);
    else
        tc_stat(TC_STAT_END | TC_STAT_FAILED);

    return 0;
}

int _tc_messageq_simple()
{
    /* 设置TestCase清理回调函数 */
  //  tc_cleanup(_tc_cleanup);
    messageq_simple_init();

    /* 返回TestCase运行的最长时间 */
    return 100;
}
/* 输出函数命令到finsh shell中 */
FINSH_FUNCTION_EXPORT_ALIAS(_tc_messageq_simple, __cmd_mq,  a simple message queue example);
