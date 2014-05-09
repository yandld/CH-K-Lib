/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
---Author-Explanation
* 
* 1.00.00 020519 JJL    First release of uC/GUI to uC/OS-II interface
* 
*
* Known problems or limitations with current version
*
*    None.
*
*
* Open issues
*
*    None
*********************************************************************************************************
*/

#include "GUI_Private.H"
#include <stdio.h>
#include <rtthread.h>


GUI_TIMER_TIME GUI_X_GetTime (void) 
{
    return (rt_tick_get()*(1000/RT_TICK_PER_SECOND));
}

void  GUI_X_Delay (int ms) 
{
    rt_uint32_t delay;
    rt_uint32_t factor;
    factor = 1000/RT_TICK_PER_SECOND; /* how many ms in a tick */
    delay = ms/factor;
    if(!delay) delay = 1;
    rt_thread_delay(delay);
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


/*
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
*             which means you have more than one thread using the uC/GUI API.  In this case the #define 
*             GUI_OS 1   needs to be in GUIConf.h
*********************************************************************************************************
*/
static rt_mutex_t gui_x_mutex = RT_NULL;

void  GUI_X_InitOS (void)
{ 
    gui_x_mutex = rt_mutex_create("gui_x_mutex", RT_IPC_FLAG_FIFO);
    if (gui_x_mutex == RT_NULL)
    {
        rt_kprintf("gui_x_mutex create failed\r\n");
        return;
    }
}

void GUI_X_Lock (void)
{ 
   // result = rt_mutex_take(gui_x_mutex, RT_WAITING_FOREVER);

}


void GUI_X_Unlock (void)
{ 
   // rt_mutex_release(gui_x_mutex);
}


U32  GUI_X_GetTaskId (void) 
{

}

void GUI_TOUCH_X_ActivateX(void) 
{

}


void GUI_TOUCH_X_ActivateY(void)
{

}


int  GUI_TOUCH_X_MeasureX(void) 
{
    rt_uint8_t buf[2];
    rt_uint16_t value;
    rt_device_t touch_device = rt_device_find("ads7843");
    if(touch_device != RT_NULL)
    {
        touch_device->read(touch_device, 0, buf, 2);
        value = ((buf[0]<<8) + buf[1])>>4; //12bit mode
    }
    return value;
}


int  GUI_TOUCH_X_MeasureY(void) 
{
    rt_uint8_t buf[2];
    rt_uint16_t value;
    rt_device_t touch_device = rt_device_find("ads7843");
    if(touch_device != RT_NULL)
    {
        touch_device->read(touch_device, 1, buf, 2);
        value = ((buf[0]<<8) + buf[1])>>4; //12bit mode
    }
    return value;
}
/*
*********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed	in the keyboard buffer. 
*          This is a neat feature which allows you to operate your target system without having to use or 
*          even to have a keyboard connected to it. (useful for demos !)
*********************************************************************************************************
*/

static  void  CheckInit (void) 
{
    GUI_X_Init();
}


void GUI_X_Init (void) 
{
  //  KeySem = OSSemCreate(0);
}
