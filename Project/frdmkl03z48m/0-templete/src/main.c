#include "gpio.h"
#include "common.h"


/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
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
    GPIO_QuickInit(HW_GPIOB, 13, kGPIO_Mode_OPP);
    /* 初始化一个串口 使用UART0端口的PTD6引脚和PTD7引脚作为接收和发送，默认设置 baud 115200 */
//    instance = UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
   // UART_CallbackRxInstall(instance, UART_ISR);
   // UART_ITDMAConfig(instance, kUART_IT_Rx, true);
   // printf("HelloWorld!\r\n", __func__);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOB, 13);
        DelayMs(500);
    }
}


