#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "wdog.h"

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU); /* KEY */
    
    /* 初始化一个模块的一般模式: 初始化模块本身->根据芯片手册 初始化对应的复用引脚->使用模块 */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.baudrate = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_Init(&UART_InitStruct1);
    
    /* 初始化串口0对应的引脚 D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /* 初始化看门狗 */
    WDOG_InitTypeDef WDOG_InitStruct1 = {0};
    WDOG_InitStruct1.mode = kWDOG_Mode_Normal;
    WDOG_InitStruct1.timeOutInMs = 1000; /* 时限 2000MS : 2000MS 内没有喂狗则复位 */
    WDOG_Init(&WDOG_InitStruct1);
    
    printf("WDOG test start! press KEY1 to feed dog\r\n");
    
    /* 点亮LED 然后熄灭  指示系统运行从新上电运行 */
    GPIO_WriteBit(HW_GPIOE, 6, 0);
    DelayMs(200);
    GPIO_WriteBit(HW_GPIOE, 6, 1);
    while(1)
    {
        if(GPIO_ReadBit(HW_GPIOE, 26) == 0) /* 按键被按下 */
        {
            /* 喂狗 防止复位 */
            WDOG_Refresh();
        }
        DelayMs(10);
    }
}


