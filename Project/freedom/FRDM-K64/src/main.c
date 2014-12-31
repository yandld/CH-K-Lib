#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "math.h"
 /*
     实验名称：UART打印信息
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用串口UART将芯片的出厂信息在芯片上电后发送出去
        发送完毕后，进入while中，执行小灯闪烁效果
*/
 
int main(void)
{
    int cnt;
    int i;
    float num;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    /* 打印芯片信息 */
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());

    while(1)
    { 
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1);
    }
}


