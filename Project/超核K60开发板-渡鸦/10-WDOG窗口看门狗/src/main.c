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
    WDOG_InitStruct1.windowInMs = 600;   /* 开窗时间 设置为窗体模式后 喂狗必须在 看门狗开始计时后 600-1000MS内完成 多了少了都复位 比普通看门狗严格*/
    WDOG_InitStruct1.timeOutInMs = 1000; /* 时限 1000MS : 1000MS 内没有喂狗则复位 */
    WDOG_Init(&WDOG_InitStruct1);
    
    printf("system reset! WDOG test start! \r\n");
    printf("press any character to disable dog feed\r\n");
    
    static uint32_t i;
    uint8_t ch;
    while(1)
    {
        if(UART_ReadByte(HW_UART0, &ch) == 0)
        {
            printf("stop feed wdog, will cause reset!\r\n");
            while(1);
        }
        printf("cnt:i:%d\r\n", i++);
        DelayMs(700); /* 喂狗时间必须在 600-1000MS内 */
        GPIO_ToggleBit(HW_GPIOE, 6);
        WDOG_Refresh();
    }
}


