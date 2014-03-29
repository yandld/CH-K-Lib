#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"
#include "lptmr.h"

/* LPTMR 作为脉冲计数时 只能测量一路脉冲 */

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* 为了代码简单 这里直接调用快速初始化了 */
    
    printf("lptrm pulse counter test\r\n");
    
    /* 在PC01 上产生一定频率的方波 便于脉冲计数测量 */
    FTM_PWM_QuickInit(FTM0_CH0_PC01, 1000);
    /* 设置FTM 的占空比 */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 5000); // 50%占空比 0-10000 对应 0-100%
    
    /* 快速初始化 LPTMR 用作脉冲捕捉 */
    LPTMR_PC_QuickInit(LPTMR_ALT2_PC05); /* PC = pulse counter 脉冲计数 */

    uint32_t value;
    while(1)
    {
        value = LPTMR_PC_ReadCounter();
        LPTMR_ClearCounter();
        printf("LPTMR:%dHz\r\n", value);
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1000);
    }
}


