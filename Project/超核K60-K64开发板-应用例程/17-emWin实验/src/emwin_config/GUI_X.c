#include "GUI_Private.H"
#include <common.h>
#include "ads7843.h"
#include "systick.h"

extern uint32_t TimerFlag;

GUI_TIMER_TIME GUI_X_GetTime (void) 
{
    return TimerFlag*10;
}

void  GUI_X_Delay (int ms) 
{
    DelayMs(ms);
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

void GUI_X_Lock (void)
{ 
    
}


void GUI_X_Unlock (void)
{ 
    
}


U32 GUI_X_GetTaskId (void) 
{
    return 1;
}



#define SAMP_CNT 8
#define SAMP_CNT_DIV2 2
static int buf[2];
/* ÂË²¨ */
static int ads_filter(int* buf)
{
    int i, j, k, min;
    int temp;
    int tempXY[2][SAMP_CNT];

    for(i=0; i<SAMP_CNT; i++)
    {
        ads7843_readX(&buf[0]);
        ads7843_readY(&buf[1]);
        tempXY[0][i] = buf[0];
        tempXY[1][i] = buf[1];
    }
    for(k=0; k<2; k++)
    {
        for(i=0; i<SAMP_CNT-1; i++)
        {
            min=i;
            for (j=i+1; j<SAMP_CNT; j++)
            {
                if (tempXY[k][min] > tempXY[k][j]) min=j;
            }
            temp = tempXY[k][i];
            tempXY[k][i] = tempXY[k][min];
            tempXY[k][min] = temp;
        }
        if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
        return 1;
        buf[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1]) / 2;
    }
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
}
