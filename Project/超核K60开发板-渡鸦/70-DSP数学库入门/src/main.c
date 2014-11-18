#include "chlib_k.h"
#include "arm_math.h"
#define PA  (3.1415926f)//
/*Refine DSP Demo**/
int main(void)
{
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);

    float32_t in = (PA*PA);
    float32_t out;
    /* 做一个开根号测试 */
    arm_sqrt_f32(in, &out);
    printf("arm_sqrt_f32:sqrt(%f) = %f\r\n",in, out);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


