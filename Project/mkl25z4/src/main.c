
#include "common.h"
#include "gpio.h"

#include "stdio.h"


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
   
//    SHELL_register_function_array(MyCommand, ARRAY_SIZE(MyCommand));
 
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

