#include "common.h"
#include "gpio.h"
#include "lpuart.h"
#include "shell.h"



static void Putc(uint8_t data)
{
    LPUART_WriteByte(HW_LPUART0, data);
}

static uint8_t Getc(void)
{
    uint8_t ch;
    while(LPUART_ReadByte(HW_LPUART0, &ch));
    return ch;
}

shell_io_install_t Shell_IOInstallStruct1 = 
{
	.getc = Getc,
	.putc = Putc,
};
extern const cmd_tbl_t CommandFun_Hist;
extern const cmd_tbl_t CommandFun_Help;
extern const cmd_tbl_t CommandFun_LPUART;
int main(void)
{
    uint8_t i,ch;
    //定义GPIO初始化结构
    //  SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
    //  DelayInit();
    // UART_DebugPortInit(UART4_RX_PC14_TX_PC15, 115200);
    // UART_printf("HelloWorld!\r\n");
    //  configure_uart_pin_mux(1);
    ch = LPUART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
    CMD_GPIO(1, NULL);
    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_Hist);
    shell_register_function(&CommandFun_LPUART);

    printf("When you see this string, It means that printf is OK!\r\n");
	//	UART_printf("%d\r\n", &configure_uart_pin_mux);
    
    // UART_ITConfig(UART4, kUART_IT_RDRF, ENABLE);
    //UART_ITConfig(UART4, kUART_IT_TC, ENABLE);
    // while(1);
    //GPIO_ITConfig(PTC,kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
   
    //SHELL_register_function_array(MyCommand, ARRAY_SIZE(MyCommand));

    
   // ch = LPUART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
 //   LPUART_printf("HelloWorld\r\n");
    

  //  LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_TxBTC, ENABLE);
  //  LPUART_ITDMAConfig(HW_LPUART0, kLPUART_IT_RxBTC, ENABLE);
    
   // UART_printf("HelloWorld,%d\r\n", ch);
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
    
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

