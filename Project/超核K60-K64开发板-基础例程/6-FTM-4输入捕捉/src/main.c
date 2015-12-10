/*
 * 实验名称：FTM输入捕捉测试
 * 实验平台：渡鸦开发板
 * 板载芯片：MK60DN512ZVLQ10
 * 实验说明：PA07模拟输出200Hz的PWM方波，PA08输入捕捉，因此需要将两引脚短接
 * 实验效果：在串口调试助手中输出 Frequency: 200Hz
 * 相关说明：因为FTM计数器为16位计数器，因此若采样的PWM波频率太低，可能会使
 * 			 程序中的InputCaptureValue变量并不是实际的计数值，因为计数器溢出
 * 			 之后又会自动装载清零，所以可以打开溢出中断功能来对溢出次数计数
 */
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"


static uint32_t InputCaptureValue;

static void FTM1_ISR(void)
{
    uint32_t clock;
    InputCaptureValue = FTM_GetChlCounter(HW_FTM1, HW_FTM_CH0);
    clock = GetClock(kBusClock);
    FTM_SetMoudleCounter(HW_FTM1, 0); /* 复位计数值 */
    InputCaptureValue = (clock/4/InputCaptureValue);  /* 频率 = FTM输入时钟/分频/计数值 */
}


int main(void)
{
    DelayInit();
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */

    printf("ftm intput capture test, pwm will be generated on PA07 and input pin: PA08\r\n");
    
    FTM_PWM_QuickInit(FTM0_CH4_PA07, kPWM_EdgeAligned, 200);
    
    /* 配置IC 功能 设置中断 */
    FTM_IC_QuickInit(FTM1_CH0_PA08, kFTM_ClockDiv128);
    FTM_IC_SetTriggerMode(HW_FTM1, HW_FTM_CH0, kFTM_IC_FallingEdge);
    FTM_CallbackInstall(HW_FTM1, FTM1_ISR);
    FTM_ITDMAConfig(HW_FTM1, kFTM_IT_CH0, true);
    
    while(1)
    {
        printf("Frequency:%6dHz\r\n", InputCaptureValue);
        GPIO_ToggleBit(HW_GPIOE, 6); //控制小灯闪烁
        DelayMs(500);
    }
}


