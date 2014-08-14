

#include <rtthread.h>
#include <rthw.h>
#include <stdint.h>

void rtthread_startup(void);

void SysTick_Handler(void)
{
	rt_interrupt_enter();
	rt_tick_increase();
	rt_interrupt_leave();
}

void DelayMs(uint32_t ms)
{
    if(ms < (1000/RT_TICK_PER_SECOND))
    {
        rt_thread_delay(1);
    }
    else
    {
        rt_thread_delay(ms/(1000/RT_TICK_PER_SECOND));
    }
}

int main(void)
{
	/* disable interrupt first */
	rt_hw_interrupt_disable();

	/* startup RT-Thread RTOS */
	rtthread_startup();

	return 0;

}


