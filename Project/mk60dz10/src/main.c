#include "sys.h"
#include "gpio.h"
#include "uart.h"

#include "systick.h"
#include "sdhc.h"
#include "clock.h"
#include "cpuidy.h"

#include "stdio.h"

#include "shell.h"


static void Putc(uint8_t data)
{
	UART_SendByte(UART4, data);
}

static uint8_t Getc(void)
{
	uint8_t ch;
  while(UART_ReceiveByte(UART4, &ch) == FALSE);
	return ch;
}



SHELL_IOInstall_Type Shell_IOInstallStruct1 = 
{
	.getc = Getc,
	.putc = Putc,
};



int CommandFun1(struct cmd_tbl_s *cmd_tp, int flag, int argc, char *const argv[])
{
	UART_printf("I am the Test CommandFun1\r\n");
	UART_printf("flag:%d\r\n", flag);
	while(argc--)
	{
		UART_printf("ARGV[%d]:%s\r\n", argc, argv[argc]);
	}
}

int CommandFun1Complete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
    uint8_t str_len;
    uint8_t found = 0;

    str_len = strlen(argv[argc-1]);
    if(!strncmp(argv[argc-1], "help", str_len))
    {
        cmdv[found] = "help";
        cmdv[found+1] = NULL;
        found++;
    }
    if(!strncmp(argv[argc-1], "hexx", str_len))
    {
        cmdv[found] = "hexx";
        cmdv[found+1] = NULL;
        found++;
    }
    return found;
}


const cmd_tbl_t MyCommand1 = 
{
	.name = "test",
	.maxargs = 5,
	.repeatable = 1,
	.cmd = CommandFun1,
	.usage = "Help on my function",
	.complete = CommandFun1Complete,
	.help = "I am long help of command1",
};

const cmd_tbl_t MyCommand2 = 
{
	.name = "ttt",
	.maxargs = 5,
	.repeatable = 1,
	.cmd = CommandFun1,
	.usage = "Help on my function2",
	.complete = CommandFun1Complete,
	.help = "I am long help of command2",
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
    
   SHELL_InsertFunction(&MyCommand1);
	 SHELL_InsertFunction(&MyCommand2);
		SHELL_IOInstall(&Shell_IOInstallStruct1);
    while(1)
		{
			//UART_printf("%d\r\n", Getc());
			main_loop();
		}
    

}





void SysTick_Handler(void)
{
	GPIO_ToggleBit(PTD, kGPIO_Pin_1);
}






