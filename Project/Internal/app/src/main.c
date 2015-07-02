#include <stdint.h>
#include <common.h>
#include "cpuidy.h"

int entry(void) __attribute__((section(".ARM.__at_0x40400")));
int entry(void)
{
    printf("I am moudle\r\n");
    uint32_t clock;

    /* 打印芯片信息 */
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());
    /* 打印时钟频率 */
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("core clock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("bus clock:%dHz\r\n", clock);
}


