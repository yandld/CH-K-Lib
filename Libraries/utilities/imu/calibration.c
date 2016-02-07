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

static int16_t gadj[3];
static int16_t inital_acc[3];
static int16_t inital_mag[3];
    
#ifndef ABS
#define ABS(a)         (((a) < 0) ? (-(a)) : (a))
#endif

#define CAL_MAGIC                   (0x5ACB)

#define CAL_GYRO_INIT               (0x00)
#define CAL_GYRO_COUNT              (0x02)
#define CAL_GYRO_FINISH             (0x03)

#define CAL_DEBUG		0
#if ( CAL_DEBUG == 1 )
#include <stdio.h>
#define CAL_TRACE	printf
#else
#define CAL_TRACE(...)
#endif

void dcal_print(struct dcal_t * dc)
{
    printf("cal data read %s", (dc->magic == 0x5ACB)?("ok!\r\n"):("err!\r\n"));
    printf("Gyro Off:%d %d %d \r\n", dc->go[0], dc->go[1], dc->go[2]);
    printf("Acc  Off:%d %d %d \r\n", dc->ao[0], dc->ao[1], dc->ao[2]);
    printf("Mag  Off:%d %d %d \r\n", dc->mo[0], dc->mo[1], dc->mo[2]);
    printf("Mag Gain:%f %f %f \r\n",    dc->mg[0], dc->mg[1], dc->mg[2]);
    printf("Mag  max:%d %d %d \r\n",     dc->m_max[0], dc->m_max[1], dc->m_max[2]);
    printf("Mag  min:%d %d %d \r\n",     dc->m_min[0], dc->m_min[1], dc->m_min[2]);
    printf("data out freq:%dHz\r\n", dc->outfrq);
    printf("User BaudRate:%d bps\r\n", dc->baudrate);
    printf("Out Data Type:%d\r\n", dc->out_data_type);
}

void dcal_reset_mag(struct dcal_t *dc)
{
    int i;
    for(i=0;i<3;i++)
    {
        dc->m_min[i] = 1000;
        dc->m_max[i] = -1000;
        dc->mg[i] = 1.000;
        dc->mo[i] = 0;
    } 
}

void dcal_init(struct dcal_t *dc)
{
    uint8_t i;
    if(dc->magic != CAL_MAGIC)
    {
        for(i=0; i<3; i++)
        {
            dc->ao[i] = 0;
            dc->go[i] = 0;
            dc->mo[i] = 0;
            dc->mg[i] = 1;
            dc->m_max[i] = -1000;
            dc->m_min[i] = 1000;  
        }
        dc->outfrq = 200;
        dc->baudrate = 115200;
    }
}

#define GYRO_SAMPLE_COUNT       (100)
#define GYRO_STATIC_LIMIT       (500)
#define ACC_STATIC_LIMIT        (10)
#define MAG_STATIC_LIMIT        (5)

static bool IsGyroStatic(int16_t *adata, int16_t *gdata, int16_t *mdata)
{
    if((ABS(gdata[0]) < GYRO_STATIC_LIMIT) && (ABS(gdata[1]) < GYRO_STATIC_LIMIT) && (ABS(gdata[2]) < GYRO_STATIC_LIMIT))
    {
        if((ABS(adata[0] - inital_acc[0]) < ACC_STATIC_LIMIT) && (ABS(adata[1] - inital_acc[1]) < ACC_STATIC_LIMIT) && (ABS(adata[2] - inital_acc[2])< ACC_STATIC_LIMIT))
        {
            if((ABS(mdata[0] - inital_mag[0]) < MAG_STATIC_LIMIT) && (ABS(mdata[1] - inital_mag[1]) < MAG_STATIC_LIMIT) && (ABS(mdata[2] - inital_mag[2]) < MAG_STATIC_LIMIT))
            {
                return true;
            }
            else
            {
                CAL_TRACE("mdata out of limit\r\n");
            }
            
        }
        else
        {
            CAL_TRACE("adata out of limit\r\n");
        }
    }
    else
    {
        CAL_TRACE("gdata out of limit\r\n");
    }
    return false;
}

void dcal_input(int16_t *adata, int16_t *gdata, int16_t *mdata)
{
    static  int32_t temp_sum[3];
    static int states = CAL_GYRO_INIT;
    static int still_count = 0;
    
    switch(states)
    {
        case CAL_GYRO_INIT:
            inital_acc[0] = adata[0];
            inital_acc[1] = adata[1];
            inital_acc[2] = adata[2];
            inital_mag[0] = mdata[0];
            inital_mag[1] = mdata[1];
            inital_mag[2] = mdata[2];
            CAL_TRACE("cal start, acc:%d %d %d\r\n", inital_acc[0], inital_acc[1], inital_acc[2]);
            CAL_TRACE("cal start, mag:%d %d %d\r\n", inital_mag[0], inital_mag[1], inital_mag[2]);
            if(IsGyroStatic(adata, gdata, mdata))
            {
                temp_sum[0] = 0;
                temp_sum[1] = 0;
                temp_sum[2] = 0;
                states = CAL_GYRO_COUNT;
                CAL_TRACE("CAL counting start...\r\n");
            }
            break;
        case CAL_GYRO_COUNT:
            if(IsGyroStatic(adata, gdata, mdata))
            {
                temp_sum[0] += gdata[0];
                temp_sum[1] += gdata[1];
                temp_sum[2] += gdata[2];
                still_count++;
            }
            else
            {
                still_count = 0;
                states = CAL_GYRO_INIT;
            }

            if(still_count == GYRO_SAMPLE_COUNT)
            {
                states = CAL_GYRO_FINISH;
            }
            break;
        case CAL_GYRO_FINISH:
            gadj[0] = temp_sum[0]/GYRO_SAMPLE_COUNT;
            gadj[1] = temp_sum[1]/GYRO_SAMPLE_COUNT;   
            gadj[2] = temp_sum[2]/GYRO_SAMPLE_COUNT;   
            CAL_TRACE("data output complete ! %d %d %d\r\n", gadj[0], gadj[1], gadj[2]);
            still_count = 0;
            states = CAL_GYRO_INIT;
            break;
        default:
            break;
    }
}

void dcal_get_gadj(int16_t *adj)
{
    adj[0] = gadj[0];
    adj[1] = gadj[1];
    adj[2] = gadj[2];
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

