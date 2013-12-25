
#include "common.h"
#include "gpio.h"
#include "lpuart.h"
#include "stdio.h"

void LPUART_ISR(uint8_t byteReceived, uint8_t * pbyteToSend, uint8_t flag)
{
    static uint8_t ch;
    if(flag == kLPUART_IT_TxBTC)
    {
        *pbyteToSend = ch;
        LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_TxBTC, DISABLE);
         GPIO_ToggleBit(HW_GPIOA, 5);
    }
    if(flag == kLPUART_IT_RxBTC)
    {
        LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_TxBTC, ENABLE);
        ch = byteReceived;
        GPIO_ToggleBit(HW_GPIOA, 16);
    }
}

int main(void)
{
	  uint8_t ch;
    uint32_t Req;
    uint8_t i;
    //定义GPIO初始化结构
    //  SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
    //  DelayInit();
    // UART_DebugPortInit(UART4_RX_PC14_TX_PC15, 115200);
    // UART_printf("HelloWorld!\r\n");
    //  configure_uart_pin_mux(1);

    GPIO_QuickInit(HW_GPIOA, 5, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 16, kGPIO_Mode_OPP);
	//	UART_printf("%d\r\n", &configure_uart_pin_mux);
    
    // UART_ITConfig(UART4, kUART_IT_RDRF, ENABLE);
    //UART_ITConfig(UART4, kUART_IT_TC, ENABLE);
    // while(1);
    //GPIO_ITConfig(PTC,kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
   
    //SHELL_register_function_array(MyCommand, ARRAY_SIZE(MyCommand));

    
    ch = LPUART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
    
    
    LPUART_CallbackInstall(HW_LPUART0, LPUART_ISR);
  //  LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_TxBTC, ENABLE);
    LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_RxBTC, ENABLE);
    
   // UART_printf("HelloWorld,%d\r\n", ch);
    while(1);
    
}

static void GPIO_ISR(uint32_t pinArray)
{
   // SHELL_printf(" array:0x%x\r\n", pinArray);
	
}


void SysTick_Handler(void)
{
	//GPIO_ToggleBit(PTD, kGPIO_Pin1);
}


void assert_failed(char * file, uint32_t line)
{
	//断言失败检测
	//UART_printf("assert_failed:line:%d %s\r\n",line,file);
	while(1);
}

