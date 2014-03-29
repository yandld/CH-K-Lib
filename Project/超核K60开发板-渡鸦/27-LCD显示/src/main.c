#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"

static void PIT_ISR(void)
{
    static uint32_t i;
    printf("enter PIT interrupt! %d\r\n", i++);
    /* 闪烁小灯 */
    GPIO_ToggleBit(HW_GPIOE, 6);
}

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
    
    /* 初始化PIT模块 */
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.chl = HW_PIT_CH0; /* 使用0号定时器 */
    PIT_InitStruct1.timeInUs = 1000*1000; /* 定时周期1S */
    PIT_Init(&PIT_InitStruct1);
    /* 注册PIT 中断回调函数 */
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    /* 开启PIT定时器中断 */
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);
    
    while(1)
    {
        /* 等待系统中断 */
    }
}


