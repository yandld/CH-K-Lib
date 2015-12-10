#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"

static uint32_t InputCaptureValue = 0;
static uint32_t InputCaptureValue1 = 0; 

static void FTM0_ISR(void)
{
	if(FTM_IsChnInterupt(HW_FTM0, HW_FTM_CH0))
	{
		InputCaptureValue = FTM_GetChlCounter(HW_FTM0, HW_FTM_CH0);
//		FTM_SetMoudleCounter(HW_FTM0, 0); /* 复位计数值 */
	}
	
	if(FTM_IsChnInterupt(HW_FTM0, HW_FTM_CH1))
	{
		InputCaptureValue1 = FTM_GetChlCounter(HW_FTM0, HW_FTM_CH1);
		FTM_SetMoudleCounter(HW_FTM0, 0); /* 复位计数值 */
	}
}


int main(void)
{
    DelayInit();
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */

    printf("calc the PWM duty, pwm will be generated on PA08 and input pin: PC01 and PC02\r\n");
    
    FTM_PWM_QuickInit(FTM1_CH0_PA08, kPWM_EdgeAligned, 200);
	FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH0, 2500);/* 25% */
    
    /* 配置IC 功能 设置中断 */
    FTM_IC_QuickInit(FTM0_CH0_PC01, kFTM_ClockDiv128);
    FTM_IC_SetTriggerMode(HW_FTM0, HW_FTM_CH0, kFTM_IC_RisingEdge);
    FTM_CallbackInstall(HW_FTM0, FTM0_ISR);
    FTM_ITDMAConfig(HW_FTM0, kFTM_IT_CH0, true);
	
	FTM_IC_QuickInit(FTM0_CH1_PC02, kFTM_ClockDiv128);
    FTM_IC_SetTriggerMode(HW_FTM0, HW_FTM_CH1, kFTM_IC_FallingEdge);
    /* FTM_CallbackInstall(HW_FTM1, FTM1_ISR); */
    FTM_ITDMAConfig(HW_FTM0, kFTM_IT_CH1, true);
    
	while(1)
    {
        /*
		printf("Frequency:%6dHz", InputCaptureValue);
		printf(" Frequency1:%6dHz\r\n", InputCaptureValue1);
		*/
		printf("Duty = %3f%% \n",((double)InputCaptureValue)/((double)InputCaptureValue1)*100.0);
        GPIO_ToggleBit(HW_GPIOE, 6); //控制小灯闪烁
        DelayMs(500);
    }
}
