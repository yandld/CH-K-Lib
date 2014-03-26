

#include <rtthread.h>
#include <rthw.h>
#include <finsh.h>


void SysTick_Handler(void)
{
	rt_interrupt_enter();
	rt_tick_increase();
	rt_interrupt_leave();
}


int main(void)
{
    
	/* disable interrupt first */
	rt_hw_interrupt_disable();

	/* startup RT-Thread RTOS */
	rtthread_startup();

	return 0;

}


