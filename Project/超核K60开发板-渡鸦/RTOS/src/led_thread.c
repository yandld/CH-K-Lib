#include <rtthread.h>
#include <rthw.h>

#include "gpio.h"
#include "gui.h"

#include "my_gui.h"

void led_thread_entry(void* parameter)
{
    GUI_PID_STATE  TouchPoint;
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
	while(1)
	{
        GUI_TOUCH_Exec();
        		GUI_TOUCH_GetState(&TouchPoint);
        		if (TouchPoint.Pressed)
		{
			if(TouchFlag == TouchUnPressed)
			{
				TouchFlag = TouchPress;
			}
		}
		else
		{
			if(TouchFlag == TouchPressed)
			{
				TouchFlag = TouchUnPress;	
			}
		}
        GUI_Exec();
        GPIO_ToggleBit(HW_GPIOE, 6);
        rt_thread_delay(2);
	}
}


