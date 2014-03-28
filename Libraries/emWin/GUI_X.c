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
#include "stdio.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include <rtthread.h>

/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/

//static  OS_EVENT  *DispSem;
//static  OS_EVENT  *EventMbox;

//static  OS_EVENT  *KeySem;
static  int        KeyPressed;
static  char       KeyIsInited;


/*
*********************************************************************************************************
*                                        TIMING FUNCTIONS
*
* Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion. 
*        Default time unit (tick), normally is 1 ms.
*********************************************************************************************************
*/



volatile GUI_TIMER_TIME OS_TimeMS;
/*
void SysTick_Handler(void)
{
    OS_TimeMS++;
    static U16 i;
    i++; i%=20;
    if(!i)
    {
        GUI_TOUCH_Exec();
    }
}
*/

GUI_TIMER_TIME  GUI_X_GetTime (void) 
{
    return rt_tick_get();
   // rt_uint32_t tick_start,tick_end;
}


void  GUI_X_Delay (int ms) 
{
    if(ms >= (1000/RT_TICK_PER_SECOND))
    {
        rt_thread_delay(ms);
    }
    else
    {
        rt_thread_delay(1);
    }
}


/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
void GUI_X_ExecIdle (void) 
{
    GUI_X_Delay(10);
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

void  GUI_X_InitOS (void)
{ 
  //  DispSem   = OSSemCreate(1);
  //  EventMbox = OSMboxCreate((void *)0);

}


void  GUI_X_Lock (void)
{ 
 //   INT8U  err;
    
    
  //  OSSemPend(DispSem, 0, &err);
}


void  GUI_X_Unlock (void)
{ 
 //   OSSemPost(DispSem);
}


U32  GUI_X_GetTaskId (void) 
{ 
  //  return ((U32)(OSTCBCur->OSTCBPrio));
}

void GUI_TOUCH_X_ActivateX(void) 
{

}


void GUI_TOUCH_X_ActivateY(void)
{

}
extern struct ads7843_device ads7843;
int  GUI_TOUCH_X_MeasureX(void) 
{
    U16 i;
    U16 x;
    U16 sum = 0;
    for(i=0;i<5;i++)
    {
      //  ads7843.readX(&ads7843, &x); 
        sum += x;
    }
    return sum/5;
}


int  GUI_TOUCH_X_MeasureY(void) 
{
    U16 i;
    U16 y;
    U16 sum = 0;
    for(i=0;i<5;i++)
    {
     //   ads7843.readY(&ads7843, &y); 
        sum += y;
    }
    return sum/5;
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
