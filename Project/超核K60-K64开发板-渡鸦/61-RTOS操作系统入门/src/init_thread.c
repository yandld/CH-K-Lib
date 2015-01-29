#include <rtthread.h>
#include "chlib_k.h"

/* 线程1 入口 */
void t1_thread_entry(void* parameter)
{
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}

/* 线程2 入口 */
void t2_thread_entry(void* parameter)
{
    GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 7);
        DelayMs(500);
    } 
}

void init_thread_entry(void* parameter)
{

    rt_thread_t tid;
    
    /* 创建线程 t1 堆栈大小256 优先级24 时间片20(没有用)*/
    tid = rt_thread_create("t1", t1_thread_entry, RT_NULL, 256, 0x24, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
  
    /* 创建线程 t2 堆栈大小256 优先级24 时间片20(没有用)*/
    tid = rt_thread_create("t2", t2_thread_entry, RT_NULL, 256, 0x24, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
   
    /* 删除这个进程 */
    tid = rt_thread_self();
    rt_thread_delete(tid); 
}

