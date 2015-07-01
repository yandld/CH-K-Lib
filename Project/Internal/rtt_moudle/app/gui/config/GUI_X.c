#include "GUI_Private.H"
#include <api.h>

static rt_device_t tch;
    
GUI_TIMER_TIME GUI_X_GetTime (void) 
{
    return (rt_tick_get()*(1000/RT_TICK_PER_SECOND));
}

void  GUI_X_Delay (int ms) 
{
    rt_thread_delay(ms);
}


/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
void GUI_X_ExecIdle (void) 
{
    GUI_X_Delay(1);
}

void  GUI_X_InitOS (void)
{
    
}

static U32 i;
void GUI_X_Lock (void)
{ 
    //rt_mutex_take(gui_x_mutex, RT_WAITING_FOREVER);
    //rt_kprintf("%d\r\n",i);
    i++;
}


void GUI_X_Unlock (void)
{ 
    i--;
    //rt_mutex_release(gui_x_mutex);
}


U32 GUI_X_GetTaskId (void) 
{
    return 1;
}

#define SAMP_CNT 3
static int buf[2];

/* ÂË²¨ */
static int ads_filter(int* buf)
{
    int i;
    rt_uint8_t buf1[4];
    
    buf[0] = 0;
    buf[1] = 0;
    
    for(i=0; i<SAMP_CNT; i++)
    {
        if(tch != RT_NULL)
        {
            tch->read(tch, 0, buf1, 4);
            buf[0] += ((buf1[0]<<8) + buf1[1])>>4; //12bit mode
            buf[1] += ((buf1[2]<<8) + buf1[3])>>4; //12bit mode
        }
    }
    buf[0] /= SAMP_CNT;
    buf[1] /= SAMP_CNT;
    return 0;
}


void GUI_TOUCH_X_ActivateX(void)
{
    
}

void GUI_TOUCH_X_ActivateY(void) 
{
    
}

int GUI_TOUCH_X_MeasureX(void) 
{
    ads_filter(buf);
    return buf[0];
}


int GUI_TOUCH_X_MeasureY(void) 
{
    return buf[1];
}

void GUI_X_ErrorOut(const char * s)
{
    GUI_DispString(s);
}


void GUI_X_Init (void) 
{
    tch = rt_device_find("ads7843");
    if(tch)
    {
        rt_device_init(tch);
        rt_device_open(tch, 0);
    }
}
