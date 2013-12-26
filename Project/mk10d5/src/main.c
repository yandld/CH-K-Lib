#include "sys.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "clock.h"
#include "cpuidy.h"

#include "stdio.h"
#include "mpu6050.h"
#include "shell.h"


extern const cmd_tbl_t CommandFun_Help;

static void Putc(uint8_t data)
{
    UART_SendByte(HW_UART1, data);
}

static uint8_t Getc(void)
{
    uint8_t ch;
    while(UART_ReadByte(HW_UART1, &ch));
    return ch;
}



shell_io_install_t Shell_IOInstallStruct1 = 
{
	.getc = Getc,
	.putc = Putc,
};




extern const cmd_tbl_t CommandFun_CPU;
extern const cmd_tbl_t CommandFun_Hist;
extern const cmd_tbl_t CommandFun_GPIO;
extern const cmd_tbl_t CommandFun_I2C;
extern const cmd_tbl_t CommandFun_MPU6050;


#pragma weak configure_uart_pin_mux
extern void configure_uart_pin_mux(uint32_t instance);
static void GPIO_ISR(uint32_t pinArray);


int main(void)
{
    uint8_t ch;
    char buf[10];
    uint32_t Req;
    uint8_t i;
    //定义GPIO初始化结构
  //  SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
	  DelayInit();
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    UART_printf("HelloWorld\r\n");
      
      
    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_GPIO);
    shell_register_function(&CommandFun_I2C);
    shell_register_function(&CommandFun_Hist);
    shell_register_function(&CommandFun_CPU);
    shell_register_function(&CommandFun_MPU6050);

//    printf("When you see this string, It means that printf is OK!\r\n");
		
		
    GPIO_QuickInit(HW_GPIOA, 1 , kGPIO_Mode_OPP);

   


 //   GPIO_CallbackInstall(HW_GPIOC, GPIO_ISR);
		
	//	GPIO_WriteBit(HW_GPIOD, kGPIO_Pin7, 0);
	//SHELL_printf("%d\r\n", GPIO_ReadBit(HW_GPIOD, kGPIO_Pin0));
	//SHELL_printf("%x\r\n", (uint32_t)PORTA_IRQHandler);

    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}



void assert_failed(char * file, uint32_t line)
{
	//断言失败检测
	shell_printf("assert_failed:line:%d %s\r\n",line,file);
	while(1);
}

