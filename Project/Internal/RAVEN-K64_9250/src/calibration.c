/**
  ******************************************************************************
  * @file    calibration.c
  * @author  YANDLD
  * @version V2.5
  * @date    2015.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    dymalic caliberation impletmentation
  ******************************************************************************
  */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "calibration.h"

#define M_MAX       (200)
#define M_MIN       (-200)

struct dcal_t
{
    int  magic;
    int16_t     m_min[3];
    int16_t     m_max[3];
    int16_t     mo[3];
    float       mg[3];
    bool        need_update;
    int16_t m_inital[3];
};

static struct dcal_t dcal;


static int is_mval_ok(int16_t data)
{
    if((data < M_MAX) && (data > M_MIN))
    {
        return 0;
    }
    return 1;
}
    
void dcal_init(int16_t *mdata)
{
    int i;
    for(i=0;i<3;i++)
    {
        dcal.m_min[i] = mdata[i];
        dcal.m_max[i] = mdata[i];
        dcal.mg[i] = 1.000;
        dcal.mo[i] = 0;
    }
}

void dcal_input(int16_t *mdata)
{
    int i;
    for(i=0;i<3;i++)
    {
        if(is_mval_ok(mdata[i]))
        {
            printf("data invailid:%d\r\n", mdata[i]);
            return;
        }
        
        if(dcal.m_max[i] < mdata[i])
        {
            dcal.m_max[i] = mdata[i];
            dcal.need_update = true;
        }
            
        if(dcal.m_min[i] > mdata[i])
        {
            dcal.m_min[i] = mdata[i];
            dcal.need_update = true;
        }
        
        dcal.mo[i] = (dcal.m_max[i] + dcal.m_min[i])/2;
    }
    
    dcal.mg[0] = 1.000;
    dcal.mg[1] = (float)(dcal.m_max[1] - dcal.m_min[1])/(float)(dcal.m_max[0] - dcal.m_min[0]);
    dcal.mg[2] = (float)(dcal.m_max[2] - dcal.m_min[2])/(float)(dcal.m_max[0] - dcal.m_min[0]);
}

void dcal_output(struct dcal_t *dc)
{
    memcpy(dc, &dcal, sizeof(struct dcal_t));
}



