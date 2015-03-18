#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>


#define SWD_INSTANCE    HW_GPIOE
#define SWD_CLK_PIN         26
#define SWD_DO_PIN          25

#define SWD_CLK             PEout(SWD_CLK_PIN)
#define SWD_O               PEout(SWD_DO_PIN)
#define SWD_I               PEin(SWD_DO_PIN)

#define PIN_DELAY()         __NOP()


void PIN_SWDIO_OUT_ENABLE(void)
{
    GPIO_PinConfig(SWD_INSTANCE, SWD_DO_PIN, kOutput);
}

void PIN_SWDIO_OUT_DISABLE(void)
{
    GPIO_PinConfig(SWD_INSTANCE, SWD_DO_PIN, kInput);
}

void PIN_SWDIO_OUT(uint32_t bit)
{
    SWD_O = bit;
}

void SW_CLOCK_CYCLE(void)
{
    SWD_CLK = 0;
    PIN_DELAY();
    SWD_CLK = 1;
    PIN_DELAY();
}

void SW_WRITE_BIT(uint32_t bit)
{
    SWD_O = bit;
    SWD_CLK = 0;
    PIN_DELAY();
    SWD_CLK = 1;
    PIN_DELAY();
}


uint32_t SW_READ_BIT(void)
{
    uint32_t bit;
    SWD_CLK = 0;
    PIN_DELAY();
    bit = SWD_I;
    SWD_CLK = 1;
    PIN_DELAY();
    return bit;
}
  
__weak void CHSW_IOInit(void)
{
    GPIO_QuickInit(SWD_INSTANCE, SWD_CLK_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(SWD_INSTANCE, SWD_DO_PIN, kGPIO_Mode_OPP);

}


