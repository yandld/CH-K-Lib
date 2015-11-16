#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */


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

/*
     实验名称：FTM输出PWM
     实验平台；凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：控制PTA端口的9引脚是PWM模式
      期初输出占空比为50%，频率为3KHz的方波 
			后期小灯的占空比逐渐变化，小灯的亮度会逐渐变化
*/
int main(void)
{
    uint16_t cnt;
   	DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("ftm test, pwm will be generated on PA09\r\n");
    
    /* 使用快速初始化 帮助初学者完成必要配置 */
    FTM_PWM_QuickInit(FTM1_CH1_PA09, kPWM_EdgeAligned, 5000);
    
    while(1)
    {
        DelayMs(5);
			  cnt = cnt+10;
			 /* 设置FTM1模块1通道的占空比 */
        FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, cnt); /* 0-10000 对应 0-100% */
			 if(cnt >= 10000) cnt = 0;
    }
}


