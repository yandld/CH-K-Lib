/**
  ******************************************************************************
  * @file    clock.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为内部文件，用于设置和获取芯片时钟频率，用户无需调用和修改  
  ******************************************************************************
  */
#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stdint.h>

/* 时钟源定义 */
typedef enum
{
    kCoreClock,
    kSystemClock,
    kBusClock,
    kFlexBusClock,
    kFlashClock,
    kClockNameCount,
}CLOCK_Source_Type; 


typedef enum
{
    kMcgOut2CoreDivider,
    kMcgOut2SystemDivider,
    kMcgOut2BusDivider,
    kMcgOut2FlashDivider,
    kClockDividerNameCount,
}CLOCK_DividerSource_Type;

typedef enum
{
    kClockSimDiv1,
    kClockSimDiv2,
    kClockSimDiv3,
    kClockSimDiv4,
    kClockSimDiv5,
    kClockSimDiv6,
    kClockSimDiv7,
    kClockSimDiv8,
    kClockSimDiv9,
    kClockSimDiv10,
    kClockSimDiv11,
    kClockSimDiv12,
    kClockSimDiv13,
    kClockSimDiv14,
    kClockSimDiv15,
    kClockSimDiv16,
}CLOCK_DivideValue_Type;

//!< API functions
int32_t CLOCK_GetClockFrequency(CLOCK_Source_Type clockName, uint32_t* FrequenctInHz);

#endif



