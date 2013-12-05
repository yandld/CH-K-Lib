#include "sys.h"
#include "gpio.h"
#include "uart.h"
#include "minishell.h"
#include "systick.h"
#include "sdhc.h"
#include "clock.h"
#include "cpuidy.h"

#include "stdio.h"

#include "shell.h"
//User defined functions
extern int CommandFun_SDInitTest(int argc, char *argv[]);
MINISHELL_CommandTableTypeDef cmd_tbl[] =
{
    { "info", 1, DisplayCPUInfo ,"list CPU info" },
};


//??MINISHELL????PutChar??
static void Putc(uint8_t data)
{
	UART_SendByte(UART4, data);
}
//??MINISHELL????GetChar??
static uint8_t Getc(void)
{
	uint8_t ch;
  while(UART_ReceiveByte(UART4, &ch) == FALSE);
	return ch;
}

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
   // GPIO_Init(&GPIO_InitStruct1);      
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
    Req = 0;
		for(i=0;i<kMemNameCount;i++)
		{
    CPUIDY_GetMemSize(i ,&Req);
		UART_printf("Mem:%d\r\n", Req);
		}

	 // UART_ITConfig(UART4, kUART_IT_RDRF, ENABLE);
	  //UART_ITConfig(UART4, kUART_IT_TC, ENABLE);
	 // while(1);
    //GPIO_ITConfig(PTC,kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
    UART_printf("FamilyID:%s\r\n", CPUIDY_GetFamID());
		CPUIDY_GetPinCount(&Req);
		UART_printf("PinCnt:%d\r\n", Req);

		
//?? MiniShell??????
MINISHELL_InstallTypeDef MiniShell_InstallStruct1 = 
{
    .ctrl_putchar = putc,
    .ctrl_getchar = getc,
};

    MINISHELL_Install(&MiniShell_InstallStruct1);
	 // MINISHELL_Init();
    while(1)
		{
			//UART_printf("%d\r\n", Getc());
			readline("MS>>");
		}
    


    while(1)
    {
			SHELL_CmdHandleLoop("SHELL>>");
    }
}





void SysTick_Handler(void)
{
	GPIO_ToggleBit(PTD, kGPIO_Pin_1);
}






