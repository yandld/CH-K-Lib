#include "clock.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "cpuidy.h"
#include "stdio.h"
#include "common.h"

#include "shell/shell.h"

static uint8_t UART_Instance;

extern const cmd_tbl_t CommandFun_Help;
extern const cmd_tbl_t CommandFun_GPIO;
extern const cmd_tbl_t CommandFun_UART;
extern const cmd_tbl_t CommandFun_CPU;
extern const cmd_tbl_t CommandFun_PIT;
extern const cmd_tbl_t CommandFun_ADC;
extern const cmd_tbl_t CommandFun_FLEXBUS;
extern const cmd_tbl_t CommandFun_FTM;
extern const cmd_tbl_t CommandFun_RESET;
extern const cmd_tbl_t CommandFun_NVIC;

static void Putc(uint8_t data)
{
	UART_WriteByte(UART_Instance, data);
}

static uint8_t Getc(void)
{
    uint8_t ch;
    while(UART_ReadByte(UART_Instance, &ch));
    return ch;
}


shell_io_install_t Shell_IOInstallStruct1 = 
{
	.sh_getc = Getc,
	.sh_putc = Putc,
};

static const QuickInit_Type FTM_QuickInitTable[] =
{
    { 0, 1, 4, 12, 1, 4}, //FTM0_CH4_PB12 4
    { 0, 1, 4, 13, 1, 5}, //FTM0_CH5_PB13  4
    { 0, 0, 3,  0, 1, 5}, //FTM0_CH5_PA00  3
    { 0, 0, 3,  1, 1, 6}, //FTM0_CH6_PA01  3
    { 0, 0, 3,  2, 1, 7}, //FTM0_CH7_PA02  3
    { 0, 0, 3,  3, 1, 0}, //FTM0_CH0_PA03  3
    { 0, 0, 3,  4, 1, 1}, //FTM0_CH1_PA04  3
    { 0, 0, 3,  5, 1, 2}, //FTM0_CH2_PA05  3
    { 0, 0, 3,  6, 1, 3}, //FTM0_CH3_PA06  3
    { 0, 0, 3,  7, 1, 4}, //FTM0_CH4_PA07  3
    { 0, 2, 4,  1, 1, 0}, //FTM0_CH0_PC01  4
    { 0, 2, 4,  2, 1, 1}, //FTM0_CH1_PC02  4
    { 0, 2, 4,  3, 1, 2}, //FTM0_CH2_PC03  4
    { 0, 2, 4,  4, 1, 3}, //FTM0_CH3_PC04  4
    { 0, 3, 4,  4, 1, 4}, //FTM0_CH4_PD04  4
    { 0, 3, 4,  5, 1, 5}, //FTM0_CH5_PD05  4
    { 0, 3, 4,  6, 1, 6}, //FTM0_CH6_PD06  4
    { 0, 3, 4,  7, 1, 7}, //FTM0_CH7_PD07  4
    { 1, 1, 3, 12, 1, 0}, //FTM1_CH0_PB12  3
    { 1, 1, 3, 13, 1, 1}, //FTM1_CH1_PB13  3
    { 1, 0, 3,  8, 1, 0}, //FTM1_CH0_PA08  3
    { 1, 0, 3,  9, 1, 1}, //FTM1_CH1_PA09  3
    { 1, 0, 3, 12, 1, 0}, //FTM1_CH0_PA12  3
    { 1, 0, 3, 12, 1, 1}, //FTM1_CH1_PA13  3
    { 1, 1, 3,  0, 1, 0}, //FTM1_CH0_PB00  3
    { 1, 1, 3,  1, 1, 1}, //FTM1_CH1_PB01  3
    { 2, 0, 3, 10, 1, 0}, //FTM2_CH0_PA10  3
    { 2, 0, 3, 11, 1, 1}, //FTM2_CH1_PA11  3
    { 2, 1, 3, 18, 1, 0}, //FTM2_CH0_PB18  3
    { 2, 1, 3, 19, 1, 1}, //FTM2_CH1_PB19  3
};

void CalConst(const QuickInit_Type * table, uint32_t size)
{
	uint8_t i =0;
	uint32_t value = 0;
	for(i = 0; i < size; i++)
	{
		value = table[i].ip_instance<<0;
		value|= table[i].io_instance<<3;
		value|= table[i].mux<<6;
		value|= table[i].io_base<<9;
		value|= table[i].io_offset<<14;
		value|= table[i].channel<<19;
		UART_printf("(0x%xU)\r\n",value);
	}
}

int main(void)
{
    DelayInit();
    //初始化 PC3 PC4 为UART1引脚 波特率115200
   // UART_Instance = UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    UART_Instance = UART_QuickInit(UART4_RX_PC14_TX_PC15,115200);
    UART_printf("HelloWorld\r\n");
      
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOD, 0, kGPIO_Mode_OPP);
    
    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_GPIO);
    shell_register_function(&CommandFun_UART);
    shell_register_function(&CommandFun_CPU);
    shell_register_function(&CommandFun_PIT);
    shell_register_function(&CommandFun_ADC);  
    shell_register_function(&CommandFun_FLEXBUS); 
    shell_register_function(&CommandFun_FTM); 
    shell_register_function(&CommandFun_RESET); 
    shell_register_function(&CommandFun_NVIC); 
    
    //CalConst(FTM_QuickInitTable, ARRAY_SIZE(FTM_QuickInitTable));
    while(1)
    {
        shell_main_loop("SHELL>>");
        DelayMs(500);
     //   UART_printf("I am CHK\r\n");
        GPIO_ToggleBit(HW_GPIOA, 1);
        GPIO_ToggleBit(HW_GPIOD, 0);
    }
}



#ifdef USE_FULL_ASSERT
void assert_failed(char * file, uint32_t line)
{
	//断言失败检测
	while(1);
}
#endif
