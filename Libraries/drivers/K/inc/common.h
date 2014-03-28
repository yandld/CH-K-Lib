/**
  ******************************************************************************
  * @file    common.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
	* @note    此文件为内部文件，用户无需调用和修改  
  ******************************************************************************
  */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#ifdef MK10D5
#include "MK10D5.h"
#elif MK10D10
#include "MK10D10.h"
#elif MK20D10
#include "MK20D10.h"
#elif MK40D10
#include "MK40D10.h"
#elif MK60D10
#include "MK60D10.h"
#elif MK60DZ10
#include "MK60DZ10.h"
#elif MK70F12
#include "MK70F12.h"
#elif MK70F15
#include "MK70F15.h"

#else
#define MK60DZ10
#include "MK60DZ10.h"
#warning "No CPU defined!"
#warning "Default: MK60DZ10"

#endif


/* means you can use printf via UART */
#define UART_USE_STDIO



/* version information */
#define CHK_VERSION                   2L              //!< major version number
#define CHK_SUBVERSION                5L              //!< minor version number
#define CHK_REVISION                  0L              //!< revise version number

/* CH_Kinetis version */
#define FW_VERSION                ((CHK_VERSION * 10000) + \
                                  (CHK_SUBVERSION * 100) + CHK_REVISION)


//参数检测器
#if defined(DEBUG)

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((char *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(char * file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif


typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {FALSE = 0, TRUE = !FALSE} ErrorState;


#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define  ABS(a)         (((a) < 0) ? (-(a)) : (a))
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))


#if !defined(NULL)
	#define NULL 0
#endif

//!< 
typedef struct
{
    uint32_t ip_instance:3;
    uint32_t io_instance:3;
    uint32_t mux:3;
    uint32_t io_base:5;
    uint32_t io_offset:5;
    uint32_t channel:5;
    uint32_t reserved:8;
}QuickInit_Type;

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

struct reg_ops
{
    void *      addr;
    uint32_t    mask;
};

#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority   4 bits for subpriority */                                               
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority   3 bits for subpriority */                                                  
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority   2 bits for subpriority */                                                   
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority   1 bits for subpriority */                                                   
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority   0 bits for subpriority */

//!< API functions
#define EnableInterrupts   __enable_irq
#define DisableInterrupts  __disable_irq
#define SystemSoftReset    NVIC_SystemReset
uint32_t QuickInitEncode(QuickInit_Type * type);
void QuickInitDecode(uint32_t map, QuickInit_Type* type);
void DelayMs(uint32_t ms);
void DelayUs(uint32_t ms);
void DelayInit(void);
int32_t CLOCK_GetClockFrequency(CLOCK_Source_Type clockName, uint32_t* FrequenctInHz);

#endif
