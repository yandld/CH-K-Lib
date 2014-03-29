#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"


/* 可用的FTM通道有: */
/*
 FTM0_CH4_PB12   // ftm0 模块 4通道 PB12引脚
 FTM0_CH5_PB13   
 FTM0_CH5_PA00   
 FTM0_CH6_PA01   
 FTM0_CH7_PA02   
 FTM0_CH0_PA03
 FTM0_CH1_PA04   
 FTM0_CH2_PA05   
 FTM0_CH3_PA06   
 FTM0_CH4_PA07   
 FTM0_CH0_PC01   
 FTM0_CH1_PC02   
 FTM0_CH2_PC03   
 FTM0_CH3_PC04   
 FTM0_CH4_PD04   
 FTM0_CH5_PD05   
 FTM0_CH6_PD06   
 FTM0_CH7_PD07   
 FTM1_CH0_PB12   
 FTM1_CH1_PB13   
 FTM1_CH0_PA08   
 FTM1_CH1_PA09   
 FTM1_CH0_PA12   
 FTM1_CH1_PA13   
 FTM1_CH0_PB00   
 FTM1_CH1_PB01   
 FTM2_CH0_PA10   
 FTM2_CH1_PA11   
 FTM2_CH0_PB18   
 FTM2_CH1_PB19   
*/

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    
    /* 初始化一个模块的一般模式: 初始化模块本身->根据芯片手册 初始化对应的复用引脚->使用模块 */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* 初始化串口0对应的引脚 D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    printf("ftm test, pwm will be generated on PC01\r\n");
    
    /* 使用快速初始化 帮助初学者完成必要配置 */
    FTM_PWM_QuickInit(FTM0_CH0_PC01, 3000);
    
//    /* 另外一种方法 普通模式 初始化FTM模块 推荐 */
//    FTM_PWM_InitTypeDef FTM_PWM_InitStruct1 = {0};
//    FTM_PWM_InitStruct1.chl = HW_FTM_CH0; /* 通道0 */
//    FTM_PWM_InitStruct1.frequencyInHZ = 3000; /* 3Khz */
//    FTM_PWM_InitStruct1.instance = HW_FTM0;
//    FTM_PWM_InitStruct1.mode = kPWM_EdgeAligned; /* 边沿对齐模式 产生普通 PWM波 */
//    FTM_PWM_Init(&FTM_PWM_InitStruct1);
//    
//    /* 初始化对应的引脚 */
//    PORT_PinMuxConfig(HW_GPIOC, 1, kPinAlt4);
    
    /* 设置FTM 的占空比 */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 5000); // 50%占空比 0-10000 对应 0-100%
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


