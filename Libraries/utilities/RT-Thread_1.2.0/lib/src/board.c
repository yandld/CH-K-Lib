#include <rtconfig.h>
#include <rtdebug.h>
#include <rtdef.h>
#include <rtservice.h>
#include <rtm.h>

#include <stdint.h>

void rtt_need_this_file(void)
{
    volatile int i;
    i = i;
}

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




