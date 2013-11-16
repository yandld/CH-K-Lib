#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include "MK60DZ10.h"

//! @addtogroup CH_Periph_Driver
//! @{

//! @addtogroup COMMON
//! @brief Common typedef for CHLib
//! @{

//! @addtogroup COMMON_Config_Macros
//! @{

//! @brief Set to 1 to expanse the "assert_param" macro in the Lib drivers code 
#define USE_FULL_ASSERT         (1)
//! @brief Debug printf and shell max char len
#define UART_PRINTF_CMD_LENGTH  (64)
//! @brief Target MCU
#define TARGET_MCU               MK60
//! @brief DELAY Function Selection
#define DelayInit              PIT_DelayInit
#define DelayMs(x)             PIT_DelayMs(x)
#define DelayUs(x)             PIT_DelayUs(x)


//! @}


//版本
#define  FW_VERSION                 (250)                //CH_K60固件库版本
//参数检测器
#if USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
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

#ifndef NULL
	#define NULL 0
#endif

//! @}


/*! @brief Status return codes.*/
typedef enum
{
    kStatus_Success = 0,
    kStatus_AbortDataPhase,
    kStatus_ReadOnly,
    kStatus_OutOfRange,
    kStatusUnknownProperty,
    kStatusInvalidArgument,
    kStatusFail,
    kStatusRomLdrSectionOverrun,
    kStatusRomLdrSignature,
    kStatusRomLdrSectionLength,
    kStatusRomLdrUnencryptedOnly,
    kStatusRomLdrEOFReached,
    kStatusRomLdrChecksum,
    kStatusRomLdrUnknownCommand,
    kStatusRomLdrIdNotFound,
    kStatusUnknownCommand,
    kStatusMemoryRangeInvalid,
    kStatusMemoryReadFailed,
    kStatusMemoryWriteFailed,
    kStatusNoMedium,
    kStatusTimeOut,
}status_t;


//Peripheral IP CODE  use for further version. No use in V2.5
#define  SYS_IP_CODE    (0)
#define GPIO_IP_CODE    (0)
#define UART_IP_CODE    (0)
#define  PIT_IP_CODE    (0)


#endif


//! @}

//! @}
