#ifndef __CH_LIB_KL_COMMON_H__
#define __CH_LIB_KL_COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef MKL25Z4
#include "MKL25Z4.h"
#elif MKL27Z4
#include "MKL27Z4.h"
#elif MKL28Z7
#include "MKL28Z7.h"
#elif  MKL26Z4
#include "MKL26Z4.h"
#elif  MKL03Z4
#include "MKL03Z4.h"
#elif  MKL46Z4
#include "MKL46Z4.h"
#elif  MKL43Z4
#include "MKL43Z4.h"
#else
#error "No CPU defined!"

#endif

/* version information */
#define CHLIB_VERSION                   (261)


#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority   4 bits for subpriority */                                               
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority   3 bits for subpriority */                                                  
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority   2 bits for subpriority */                                                   
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority   1 bits for subpriority */                                                   
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority   0 bits for subpriority */

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)         (((a) < 0) ? (-(a)) : (a))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#ifndef BSWAP_16
#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif

#ifndef BSWAP_32
#define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))
#endif

/* init type */
typedef struct
{
    uint32_t ip:3;
    uint32_t io:3;
    uint32_t mux:3;
    uint32_t pin_start:5;
    uint32_t pin_count:5;
    uint32_t chl:5;
    uint32_t reserved:8;
}map_t;

/* clock */
typedef enum
{
    kCoreClock,
    kBusClock,
    kFlashClock,
    kFlexBusClock,
}Clock_t; 

typedef struct
{
    void *      addr;
    uint32_t    mask;
    uint32_t    shift;
}Reg_t;

#define CLK_EN(t, x)               (*((uint32_t*) t[x].addr) |= t[x].mask)
#define CLK_DIS(t, x)              (*((uint32_t*) t[x].addr) &= ~t[x].mask)
#define REG_GET(t, x)              ((*(uint32_t*) t[x].addr & t[x].mask)>>t[x].shift)

//!< API functions
void SystemSoftReset(void);
void SetPinMux(uint32_t instance, uint32_t pin, uint32_t mux);
void SetPinPull(uint32_t instance, uint32_t pin, uint32_t val);
void DelayInit(void);
void DelayMs(uint32_t ms);
void DelayUs(uint32_t ms);
uint32_t GetClock(Clock_t clock);

#endif
