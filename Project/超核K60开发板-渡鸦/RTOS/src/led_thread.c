#include <rtthread.h>
#include <rthw.h>



void led_thread_entry(void* parameter)
{

	while(1)
	{
        rt_thread_delay(100);
	}
}


