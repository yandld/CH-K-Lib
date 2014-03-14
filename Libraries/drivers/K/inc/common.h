#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdbool.h>
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


//! @brief expanse the "assert_param" macro in the Lib drivers code 
#define USE_FULL_ASSERT         
//! @brief means you can use printf
#define UART_USE_STDIO



//!< CH_Kinetis version information
#define CHK_VERSION                   2L              //!< major version number
#define CHK_SUBVERSION                5L              //!< minor version number
#define CHK_REVISION                  0L              //!< revise version number

/* CH_Kinetis version */
#define FW_VERSION                ((CHK_VERSION * 10000) + \
                                  (CHK_SUBVERSION * 100) + CHK_REVISION)


//²ÎÊý¼ì²âÆ÷
#ifdef USE_FULL_ASSERT

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
#endif /* USE_FULL_ASSERT */


typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {FALSE = 0, TRUE = !FALSE} ErrorState;


#if !defined(MIN)
    #define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
    #define MAX(a, b)   ((a) > (b) ? (a) : (b))
#endif

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

#define BSWAP_16(x)	(uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
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

//!< 
typedef struct
{
    void * register_addr;
    uint32_t    mask;
}RegisterManipulation_Type;

#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority   4 bits for subpriority */                                               
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority   3 bits for subpriority */                                                  
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority   2 bits for subpriority */                                                   
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority   1 bits for subpriority */                                                   
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority   0 bits for subpriority */

//! @brief DELAY Function Selection
#define DelayInit              SYSTICK_DelayInit
#define DelayMs(x)             SYSTICK_DelayMs(x)
#define DelayUs(x)             SYSTICK_DelayUs(x)
#define EnableInterrupts   __enable_irq
#define DisableInterrupts  __disable_irq
#define SystemSoftReset    NVIC_SystemReset
#define OSEnterInt(x)
#define OSExitInt(x)

#endif



/*
void CalConst(const QuickInit_Type * table, uint32_t size)
{
	uint8_t i =0;
	uint32_t value = 0;
	for(i = 0; i < size; i++)
	{
		value = table[i].ip_instance<<0;
		value|= table[i].io_instance<<3;
		value|= table[i].mux<<6;
		value|= table[i].io_base<<9;
		value|= table[i].io_offset<<14;
		value|= table[i].channel<<19;
		UART_printf("(0x%xU)\r\n",value);
	}
}
*/
