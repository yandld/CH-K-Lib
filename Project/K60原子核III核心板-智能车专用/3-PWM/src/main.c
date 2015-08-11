#include "gpio.h"
#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "ftm.h"
#include "ssd1306.h"


int main(void)
{
    DelayInit();
    DelayMs(10);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);

    printf("ftm test, pwm will be generated on PA06\r\n");
    
    
    FTM_PWM_QuickInit(FTM0_CH3_PA06, kPWM_EdgeAligned, 5000);
    
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, 6000); /* 0 - 10000  to 0% - 100% */
    
    while(1)
    {
        PEout(0) = !PEout(0);
        DelayMs(500);
    }
}


