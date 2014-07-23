#include "gpio.h"
#include "common.h"


 
 void UART_ISR(uint16_t byteReceived)
 {
   //  printf("%c", byteReceived);
 }
 
int main(void)
{
    uint32_t instance;
    uint16_t ch;
    DelayInit();
    /* 初始化一个引脚 设置为推挽输出 */
    GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 29, kGPIO_Mode_OPP);
    /* 初始化一个串口 使用UART0端口的PTD6引脚和PTD7引脚作为接收和发送，默认设置 baud 115200 */
//    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
   // UART_CallbackRxInstall(instance, UART_ISR);
   // UART_ITDMAConfig(instance, kUART_IT_Rx, true);
   // printf("HelloWorld!\r\n", __func__);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOD, 5);
        GPIO_ToggleBit(HW_GPIOE, 29);
        DelayMs(500);
    }
}


