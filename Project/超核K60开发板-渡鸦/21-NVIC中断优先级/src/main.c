#include "gpio.h"
#include "common.h"
#include "uart.h"


/*
连接 PE26 和 PA4 使他们的信号相同 按下KEY1
使用串口观察哪个中断 先发生 由于 PORTA PORTE的中断抢占优先级不同
抢占优先级低的不能打断抢占优先级高的中断 所以 在一个中断完全执行完后 才能出发另外一个等待的中断 
*/

static void PORTE_ISR(uint32_t array)
{
    printf("PORTE Interrupt!\r\n");
    DelayMs(1000);
}

static void PORTA_ISR(uint32_t array)
{
    printf("PORTA Interrupt!\r\n");
    DelayMs(1000);
}

/* 将E26和 A4 跳线连接 然后配置 他们的NVIC优先级 */

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    /* 设置PORTE PORTA 中断 */
    GPIO_QuickInit(HW_GPIOE,26, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_IPU);
    GPIO_CallbackInstall(HW_GPIOE, PORTE_ISR);
    GPIO_CallbackInstall(HW_GPIOA, PORTA_ISR);
    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_RisingEdge);
    GPIO_ITDMAConfig(HW_GPIOA, 4,  kGPIO_IT_RisingEdge);
    
    printf("NVIC test\r\n");
    /* 将系统 中断优先级分组 可以配置 16个 抢占优先级 和16个 子优先级 */
    NVIC_SetPriorityGrouping(3);
    printf("NVIC_PriorityGrouping:%d\r\n", NVIC_GetPriorityGrouping());
    /* 将PORTE中断设置为 抢占优先级1 子优先级2  (优先级数字越小代表优先级越高)*/
    NVIC_SetPriority(PORTE_IRQn, NVIC_EncodePriority(3, 1, 2));
    /* 将PORTE中断设置为 抢占优先级2 子优先级2  */
    NVIC_SetPriority(PORTA_IRQn, NVIC_EncodePriority(3, 2, 2)); 
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


