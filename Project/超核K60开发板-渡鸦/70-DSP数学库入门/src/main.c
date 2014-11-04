#include "chlib_k.h"
#include "arm_math.h"

 
int main(void)
{
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);

    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());

    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("core clock:%dHz\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("bus clock:%dHz\r\n", clock);
    
    float32_t in = (3.1415926*3.1415926);
    float32_t out;
    /* ¿ª¸ú */
    arm_sqrt_f32(in, &out);
    printf("arm_sqrt_f32:%f\r\n", out);
    
    while(1)
    {

        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


