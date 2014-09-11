#include <rtthread.h>
#include <rthw.h>

#include "gpio.h"
#include "gui.h"

#include "my_gui.h"

void led_thread_entry(void* parameter)
{
    GPIO_QuickInit(HW_GPIOB, 21, kGPIO_Mode_OPP);
	while(1)
	{
        //GUI_Exec();
        //GUI_TOUCH_Exec();
        GPIO_ToggleBit(HW_GPIOB, 21);
        rt_thread_delay(10);
	}
}


