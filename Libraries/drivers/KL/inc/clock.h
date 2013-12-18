#ifndef __CLOCK_H__
#define __CLOCK_H__
#include "common.h"

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

void CLOCK_GetClockFrequency(CLOCK_Source_Type clockName, uint32_t* FrequenctInHz);

#endif



