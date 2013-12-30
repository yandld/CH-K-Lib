#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef MKL25Z4
#include "MKL25Z4.h"
#endif


//! @defgroup CHKinetis-KL
//! @{

//! @addtogroup COMMON
//! @brief Common typedef for CHLib
//! @{

//! @addtogroup COMMON_Config_Macros
//! @{

//! @brief Set to 1 to expanse the "assert_param" macro in the Lib drivers code 
#define USE_FULL_ASSERT         
//! @brief means you can use printf when uart has benn initalized
#define USE_STDIO
#define UART_AUTO_LINK_CONSULT

//! @}

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


//! @addtogroup COMMON_Typedef
//! @{

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

//! @}

typedef struct
{
    uint32_t ip_instance:3;
    uint32_t io_instance:3;
    uint32_t mux:3;
    uint32_t io_base:5;
    uint32_t io_offset:5;
    uint32_t channel:5;
}QuickInit_Type;



//! @brief DELAY Function Selection
#define DelayInit              SYSTICK_DelayInit
#define DelayMs(x)             SYSTICK_DelayMs(x)
#define DelayUs(x)             SYSTICK_DelayUs(x)
#define EnableInterrupts   __enable_irq
#define DisableInterrupts  __disable_irq
#define SystemSoftReset    NVIC_SystemReset
#define OSEnterInt(x)
#define OSExitInt(x)



typedef uint8_t (*ConsultGetc_CallBackType)(void);
typedef void (*ConsultPutc_CallBackType)(uint8_t ch);

//API functions
void ConsultSelcet(ConsultGetc_CallBackType  cgetc, ConsultPutc_CallBackType  cputc);
int printf(const char *fmt, ...);

#endif


//! @}

//! @}
