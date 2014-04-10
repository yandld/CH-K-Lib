#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "wdog.h"



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
    
    /* 初始化看门狗 */
    WDOG_InitTypeDef WDOG_InitStruct1 = {0};
    WDOG_InitStruct1.mode = kWDOG_Mode_Window;
    WDOG_InitStruct1.windowInMs = 1000;   /* 开窗时间 设置为窗体模式后 喂狗必须在 看门狗开始计时后 1000 - 2000 MS内完成 多了少了都复位 比普通看门狗严格*/
    WDOG_InitStruct1.timeOutInMs = 2000; /* 时限 2000MS : 2000MS 内没有喂狗则复位 */
    WDOG_Init(&WDOG_InitStruct1);
    
    printf("\r\nSYSTEM RESET!!!!!!!%d\r\n", WDOG_GetResetCounter());
    printf("press any character to feed dog feed, must be in windows time\r\n");
    
    static uint32_t i;
    uint16_t ch;
    while(1)
    {
        if(UART_ReadByte(HW_UART0, &ch) == 0)
        {
            printf("wdog feed succ!\r\n");
            WDOG_Refresh();
            i = 0;
        }
        printf("cnt:i:%d\r", i++);
        DelayMs(100);
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


