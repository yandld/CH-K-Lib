#include "gpio.h"
#include "common.h"
/*
     实验名称：DWT延时实验
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：时间间隔是500毫秒 
*/
 
int main(void)
{
    /* 初始化Delay */
    DelayInit();
    DWT_DelayInit();

    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);

    GPIO_WriteBit(HW_GPIOA, 9, 0);
    
    while(1)
    {
        PAout(9) = !PAout(9);
        DWT_DelayMs(500);
    }
}


