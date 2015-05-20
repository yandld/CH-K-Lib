/**
  ******************************************************************************
  * @file    calibration.c
  * @author  YANDLD
  * @version V2.5
  * @date    2015.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include <string.h>

#include "calibration.h"

struct dcal_t
{
    int  magic;
    uint16_t m_min[3];
    uint16_t m_max[3];

};

static struct dcal_t dcal;

void dcal_input(int16_t *mdata)
{
    
    
    
}





