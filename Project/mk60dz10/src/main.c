#include "sys.h"
#include "gpio.h"
#include "uart.h"
#include "minishell.h"
#include "systick.h"
#include "sdhc.h"
#include "clock.h"
//User defined functions
extern int CommandFun_SDInitTest(int argc, char *argv[]);
extern int CommandFun_SDReadSingleBlockTest(int argc, char *argv[]);
extern int CommandFun_PITInitTest(int argc, char *argv[]);
extern int CommandFun_WDOGInitTest(int argc, char *argv[]);

MINISHELL_CommandTableTypeDef cmd_tbl[] =
{
    { "info", 1, DisplayCPUInfo ,"list CPU info" },
};

#pragma weak configure_uart_pin_mux
extern void configure_uart_pin_mux(uint32_t instance);
int main(void)
{
	  uint8_t ch;
    uint32_t Req;
    uint8_t i;
    //定义GPIO初始化结构
    GPIO_InitTypeDef GPIO_InitStruct1;
  //  SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
	  DelayInit();
    UART_DebugPortInit(UART4_RX_PC14_TX_PC15, 115200);
    UART_printf("HelloWorld!\r\n");
	  configure_uart_pin_mux(1);
	  MINISHELL_Register(cmd_tbl, ARRAY_SIZE(cmd_tbl));
    GPIO_InitStruct1.GPIOx = PTC;                             //C端口
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;                //初始化后输出高电平
    GPIO_InitStruct1.GPIO_Pin = kGPIO_Pin_18;                  //PC16引脚
    GPIO_InitStruct1.GPIO_Mode = kGPIO_Mode_IPU;               //推挽输出
    //执行GPIO初始化
    GPIO_Init(&GPIO_InitStruct1);      
		GPIO_ITConfig(PTC, kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
	  
    GPIO_InitStruct1.GPIOx = PTD;                             //C端口
    GPIO_InitStruct1.GPIO_InitState = Bit_SET;                //初始化后输出高电平
    GPIO_InitStruct1.GPIO_Pin = kGPIO_Pin_1;                  //PC16引脚
    GPIO_InitStruct1.GPIO_Mode = kGPIO_Mode_OPP;               //推挽输出
    //执行GPIO初始化
    GPIO_Init(&GPIO_InitStruct1);     
		
		UART_printf("%d\r\n", &configure_uart_pin_mux);
    
		for(i=0;i<kClockNameCount;i++)
		{
    CLOCK_GetClockFrequency(i ,&Req);
		UART_printf("Req:%d\r\n", Req);
		}

		
	 // UART_ITConfig(UART4, kUART_IT_RDRF, ENABLE);
	  //UART_ITConfig(UART4, kUART_IT_TC, ENABLE);
	 // while(1);

    //GPIO_ITConfig(PTC,kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
		
    //CommandFun_WDOGInitTest2();
		while(1)
		{
			DelayMs(300);
			DelayMs(300);
			UART_printf("!!\r\n");
			
		}
		
    while(1)
    {
			SHELL_CmdHandleLoop("SHELL>>");
    }
}


void assert_failed(uint8_t* file, uint32_t line)
{
	
}


void SysTick_Handler(void)
{
	GPIO_ToggleBit(PTD, kGPIO_Pin_1);
}






