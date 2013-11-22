#ifndef __MCG_H__
#define __MCG_H__


//! @brief [SystemClockUpdate] <clockSource> selection enumeration
typedef enum
{
	kClockSource_IRC,               //!< Interal ClockSource
  kClockSource_EX8M,              //!< External ClockSource 8MHZ
	kClockSource_EX50M,             //!< External ClockSource 50MHZ
}SYS_ClockSourceSelect_TypeDef; 







#endif