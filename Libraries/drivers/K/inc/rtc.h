/**
  ******************************************************************************
  * @file    rtc.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>
#include "common.h"


typedef struct 
{
   uint16_t year;    /*!< Range from 200 to 2099.*/
   uint16_t month;   /*!< Range from 1 to 12.*/
   uint16_t day;     /*!< Range from 1 to 31 (depending on month).*/
   uint16_t hour;    /*!< Range from 0 to 23.*/
   uint16_t minute;  /*!< Range from 0 to 59.*/
   uint8_t second;   /*!< Range from 0 to 59.*/

}RTC_DataTime_Type;

//!< mcu select type
typedef enum
{
    kPinAlt0,
    kPinAlt1,
    kPinAlt2,
    kPinAlt3,
}RTC_OscLoad_Type;

typedef struct
{
    RTC_OscLoad_Type oscLoad;
    
}RTC_InitTypeDef;


typedef struct 
{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;			
	uint8_t Month;
	uint8_t Date;
	uint8_t Week;	
	uint16_t Year;
	uint32_t TSRValue;
}RTC_CalanderTypeDef;		

//RTC ÖÐ¶ÏÔ´
#define RTC_IT_TAF    (uint16_t)(0)
#define RTC_IT_TOF    (uint16_t)(1)
#define RTC_IT_TIF    (uint16_t)(2)
#define IS_RTC_IT(IT)  (((IT) == RTC_IT_TAF) || \
                        ((IT) == RTC_IT_TOF) || \
                        ((IT) == RTC_IT_TIF))


/*
   *    value 0   => to be configured: 0pF \n
   *    value 2   => to be configured: 2pF \n
   *    value 4   => to be configured: 4pF \n
   *    value 8   => to be configured: 8pF \n
   *    value 16  => to be configured: 16pF \n
*/

//API functions
void RTC_Init(void);
void RTC_SecondIntProcess(void);
void RTC_GetCalander(RTC_CalanderTypeDef * RTC_CalanderStruct);
uint8_t RTC_SetCalander(RTC_CalanderTypeDef * RTC_CalanderStruct);
void RTC_ITConfig(uint16_t RTC_IT, FunctionalState NewState);

#endif
