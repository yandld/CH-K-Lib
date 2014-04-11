#include "gpio.h"
#include "common.h"
#include "uart.h"

/*
     实验名称：GPIO外部引脚中断
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：一颗LED灯规则的闪烁，当按键KEY2每按一次另一颗小灯将闪烁一次
          通过按键控制小灯的亮灭。对按键的引脚采用上升沿触发中断的方式。
*/

/* 回调函数中的 array 的32位每一位代表一个引脚，1为 该脚中断被触发 0代表该脚中断未触发 */
static void GPIO_ISR(uint32_t array)
{
    printf("KEY:0x%X\r\n", array);
    if(array & (1<<26)) /*对应的按键中断 翻转对应的LED电平 */
    {
        GPIO_ToggleBit(HW_GPIOE, 12);
    }
}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP); /* LED */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU); /* KEY */
    
    /* 初始化一个模块的一般模式: 初始化模块本身->根据芯片手册 初始化对应的复用引脚->使用模块 */
    UART_InitTypeDef UART_InitStruct1 = {0};
    UART_InitStruct1.instance = HW_UART0;
    UART_InitStruct1.baudrate = 115200;
    UART_InitStruct1.parityMode = kUART_ParityDisabled;
    UART_InitStruct1.bitPerChar = kUART_8BitsPerChar;
    UART_Init(&UART_InitStruct1);
    
    /* 初始化串口0对应的引脚 D6 D7*/
    PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
    
    /* 设置GPIO外部引脚中断回调函数 */
    GPIO_CallbackInstall(HW_GPIOE, GPIO_ISR);
    /* 打开GPIO引脚中断 上升沿触发 */
    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_RisingEdge);
    
    printf("press any key1 to let LED toggle\r\n");
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


