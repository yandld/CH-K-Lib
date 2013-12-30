#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "shell.h"

static uint8_t shell_instance;

static void Putc(uint8_t data)
{
    UART_WriteByte(shell_instance, data);
}

static uint8_t Getc(void)
{
    uint8_t ch;
    while(UART_ReadByte(shell_instance, &ch));
    return ch;
}

shell_io_install_t Shell_IOInstallStruct1 = 
{
	.getc = Getc,
	.putc = Putc,
};
extern const cmd_tbl_t CommandFun_Hist;
extern const cmd_tbl_t CommandFun_Help;
extern const cmd_tbl_t CommandFun_UART;
extern const cmd_tbl_t CommandFun_GPIO;
int main(void)
{
    uint8_t i,ch;
    //定义GPIO初始化结构
    // use UART0_RX_PA15_TX_PA14 as shell output 
   // ch = LPUART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
   //UART2_RX_PE23_TX_PE22
   //UART0_RX_PA15_TX_PA14
    shell_instance = UART_QuickInit(UART2_RX_PE23_TX_PE22, 115200);

    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_Hist);
    shell_register_function(&CommandFun_UART);
    shell_register_function(&CommandFun_GPIO);
    

    printf("When you see this string, It means that printf is OK!\r\n");
  //  UART_QuickInit(UART2_RX_PE23_TX_PE22, 115200);
  //  printf("When you see this string, It means that printf is OK!\r\n");
   // UART_printf("HelloWorld,%d\r\n", ch);
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
    
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

