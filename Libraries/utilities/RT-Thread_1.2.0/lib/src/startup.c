#include <rtthread.h>
#include <stdint.h>
#include <rthw.h>
#include "components.h"
#include "MK60D10.h"

#define RTT_IRAM_HEAP_START               (0x1FFF0000)
#define RTT_IRAM_HEAP_END                 (0x1FFF0000 + 0x10000)


void rt_application_init(void);

void rtthread_startup(void)
{
	rt_show_version();
	rt_system_timer_init();
    rt_system_scheduler_init();

    rt_device_init_all();
    rt_system_timer_thread_init();
    rt_thread_idle_init();
    rt_application_init();
    rt_system_scheduler_start();
}



int main(void)
{
	rt_hw_interrupt_disable();
    rt_system_heap_init((void*)RTT_IRAM_HEAP_START, (void*)RTT_IRAM_HEAP_END);
    
    /* init systick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND - 1);
    /* set pend exception priority */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    
    rt_components_board_init();
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
	rtthread_startup();
    
	return 0;
}





