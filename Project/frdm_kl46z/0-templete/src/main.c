#include "gpio.h"
#include "common.h"
#include "uart.h"

 
 void UART_ISR(uint16_t byteReceived)
 {
   //  printf("%c", byteReceived);
 }
 


int main(void)
{
    uint32_t i;
    uint32_t instance;
    uint16_t ch;
    uint32_t clock;
    DelayInit();
    /* 初始化一个引脚 设置为推挽输出 */
    GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 29, kGPIO_Mode_OPP);
    
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    printf("CoreClock:%d\r\n", clock);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    printf("kBusClock:%d\r\n", clock);
  //  UART_QuickInit(UART2_RX_PE17_TX_PE16, 115200);
    
    printf("HelloWorld\r\n");

    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOD, 5);
        GPIO_ToggleBit(HW_GPIOE, 29);
       // if(!UART_ReadByte(HW_UART2, &ch))
        {
            printf("%c", ch);
        }
        DelayMs(50);
    }
}


