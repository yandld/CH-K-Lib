#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>

#define TRESET_INSTANCE     (HW_GPIOB)
#define SWD_INSTANCE        (HW_GPIOC)
#define SWD_CLK_PIN         (5)
#define SWD_DO_PIN          (6)
#define TRESET_PIN          (1)

#define SWD_CLK             PCout(SWD_CLK_PIN)
#define SWD_OUT             PCout(SWD_DO_PIN)
#define SWD_READ            PCin(SWD_DO_PIN)
#define TRESET_OUT          PBout(TRESET_PIN)

#define PIN_DELAY()         DelayUs(1)

#define SWDIO_DDR_OUT()       GPIO_PinConfig(SWD_INSTANCE, SWD_DO_PIN, kOutput)
#define SWDIO_DDR_IN()        GPIO_PinConfig(SWD_INSTANCE, SWD_DO_PIN, kInput)

static inline void PIN_SWDIO_OUT(uint32_t bit)
{
    SWD_OUT = bit;
}

static inline void SW_CLOCK_CYCLE(void)
{
    SWD_CLK = 0;
    PIN_DELAY();
    SWD_CLK = 1;
    PIN_DELAY();
}

static inline void SW_WRITE_BIT(uint32_t bit)
{
    SWD_OUT = bit;
    SWD_CLK = 0;
    PIN_DELAY();
    SWD_CLK = 1;
    PIN_DELAY();
}

static inline uint32_t SW_READ_BIT(void)
{
    uint32_t bit;
    SWD_CLK = 0;
    PIN_DELAY();
    bit = SWD_READ;
    SWD_CLK = 1;
    PIN_DELAY();
    return bit;
}
  
static inline void treset_pin_control(uint32_t val)
{
    TRESET_OUT = val & 1;
}

static inline void swd_io_init(void)
{
    GPIO_QuickInit(SWD_INSTANCE, SWD_CLK_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(SWD_INSTANCE, SWD_DO_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(TRESET_INSTANCE, TRESET_PIN, kGPIO_Mode_OOD);
}



