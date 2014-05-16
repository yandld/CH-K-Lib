

#include <rtthread.h>
#include <rthw.h>
#include <finsh.h>
#include <stdint.h>

void SysTick_Handler(void)
{
	rt_interrupt_enter();
	rt_tick_increase();
	rt_interrupt_leave();
}

void DelayMs(uint32_t ms)
{ 
    rt_thread_delay(ms/10 + 1);
}


int main(void)
{
    
	/* disable interrupt first */
	rt_hw_interrupt_disable();

	/* startup RT-Thread RTOS */
	rtthread_startup();

	return 0;

}


