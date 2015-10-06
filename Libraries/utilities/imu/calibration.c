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

#define CAL_MAGIC           (0x5ACB)

#define CAL_GYRO_INIT               (0x00)
#define CAL_GYRO_COUNT              (0x02)
#define CAL_GYRO_FINISH             (0x03)

void dcal_print(struct dcal_t * dc)
{
    printf("cal data read %s", (dc->magic == 0x5ACB)?("ok!\r\n"):("err!\r\n"));
    printf("Gyro Off:%d %d %d \r\n", dc->go[0], dc->go[1], dc->go[2]);
    printf("Acc  Off:%d %d %d \r\n", dc->ao[0], dc->ao[1], dc->ao[2]);
    printf("Mag  Off:%d %d %d \r\n", dc->mo[0], dc->mo[1], dc->mo[2]);
    printf("Mag Gain:%f %f %f \r\n",    dc->mg[0], dc->mg[1], dc->mg[2]);
    printf("Mag  max:%d %d %d \r\n",     dc->m_max[0], dc->m_max[1], dc->m_max[2]);
    printf("Mag  min:%d %d %d \r\n",     dc->m_min[0], dc->m_min[1], dc->m_min[2]);
}

void dcal_reset_mag(struct dcal_t *dc)
{
    int i;
    for(i=0;i<3;i++)
    {
        dc->m_min[i] = 0;
        dc->m_max[i] = 0;
        dc->mg[i] = 1.000;
        dc->mo[i] = 0;
    } 
}

void dcal_init(struct dcal_t *dc)
{
    
}

#define GYRO_SAMPLE_COUNT       (64)
#define GYRO_STILL_LIMIT        (30)
static int32_t gsum[3];

void dcal_ginput(struct dcal_t *dc, int16_t *gdata)
{
    static int states = CAL_GYRO_INIT;
    static int still_count = 0;
    switch(states)
    {
        case CAL_GYRO_INIT:
            if((gdata[0] < GYRO_STILL_LIMIT) && (gdata[1] < GYRO_STILL_LIMIT) && (gdata[2] < GYRO_STILL_LIMIT))
            {
                still_count++;
            }
            else
            {
                still_count = 0;
                states = CAL_GYRO_INIT;
            }
            if(still_count > 20)
            {
                states = CAL_GYRO_COUNT;
                still_count = 0;
                gsum[0] = 0;
                gsum[1] = 0;
                gsum[2] = 0;
            }
            break;
        case CAL_GYRO_COUNT:
            if((gdata[0] < GYRO_STILL_LIMIT) && (gdata[1] < GYRO_STILL_LIMIT) && (gdata[2] < GYRO_STILL_LIMIT))
            {
                //printf("! %d %d %d\r\n", gdata[0], gdata[1], gdata[2]);
                gsum[0] += gdata[0];
                gsum[1] += gdata[1];
                gsum[2] += gdata[2];
                still_count++;
            }
            else
            {
                states = CAL_GYRO_INIT;
            }

            if(still_count == GYRO_SAMPLE_COUNT)
            {
                states = CAL_GYRO_FINISH;
            }
            break;
        case CAL_GYRO_FINISH:
            dc->go[0] = gsum[0]/GYRO_SAMPLE_COUNT;
            dc->go[1] = gsum[1]/GYRO_SAMPLE_COUNT;   
            dc->go[2] = gsum[2]/GYRO_SAMPLE_COUNT;   
            //printf("data output complete ! %d %d %d\r\n", gsum[0]/GYRO_SAMPLE_COUNT, gsum[1]/GYRO_SAMPLE_COUNT, gsum[2]/GYRO_SAMPLE_COUNT);
            states = CAL_GYRO_INIT;
            break;
        default:
            break;
    }
    
}



/* this function must be called every 100 ms */
void dcal_minput(struct dcal_t *dc, int16_t *mdata)
{
    int i;
    for(i=0;i<3;i++)
    {
        if(dc->m_max[i] < mdata[i])
        {
            dc->m_max[i] = mdata[i];
        }
            
        if(dc->m_min[i] > mdata[i])
        {
            dc->m_min[i] = mdata[i];
        }
        
        dc->mo[i] = (dc->m_max[i] + dc->m_min[i])/2;
    }
    
    dc->mg[0] = 1.000;
    dc->mg[1] = (float)(dc->m_max[1] - dc->m_min[1])/(float)(dc->m_max[0] - dc->m_min[0]);
    dc->mg[2]  = (float)(dc->m_max[2] - dc->m_min[2])/(float)(dc->m_max[0] - dc->m_min[0]);
        
}

void dcal_output(struct dcal_t *dc)
{
    dc->magic = CAL_MAGIC;
}

