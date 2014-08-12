#include <rtthread.h>
#include <rthw.h>

#include "gpio.h"
#include "gui.h"


void led_thread_entry(void* parameter)
{
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
	while(1)
	{
        GPIO_ToggleBit(HW_GPIOE, 6);
        rt_thread_delay(10);
	}
}


