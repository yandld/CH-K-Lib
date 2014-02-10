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
const QuickInit_Type UART_QuickInitTable[] =
{
    { 0, 0, 0, 0, 0, 0, 0}, //ADC0_SE0_DP0
    { 0, 0, 0, 0, 0, 1, 0}, //ADC0_SE1_DP1
    { 0, 0, 0, 0, 0, 3, 0}, //ADC0_SE3_DP3
    { 0, 2, 0, 2, 1, 4, 1}, //ADC0_SE4B_PC2
    { 0, 3, 0, 1, 1, 5, 1}, //ADC0_SE5B_PD1
    { 0, 3, 0, 5, 1, 6, 1}, //ADC0_SE6B_PD5
    { 0, 3, 0, 6, 1, 7, 1}, //ADC0_SE7B_PD6
    { 0, 1, 0, 0, 1, 8, 0}, //ADC0_SE8_PB0
    { 0, 1, 0, 1, 1, 9, 0}, //ADC0_SE9_PB1
    { 0, 1, 0, 2, 1, 12,0}, //ADC0_SE12_PB2
    { 0, 1, 0, 3, 1, 13,0}, //ADC0_SE13_PB3
    { 0, 2, 0, 0, 1, 14,0}, //ADC0_SE14_PC0
    { 0, 2, 0, 0, 1, 15,0}, //ADC0_SE15_PC1
    { 0, 4, 0,24, 1, 17,0}, //ADC0_SE17_E24
    { 0, 4, 0,25, 1, 18,0}, //ADC0_SE18_E25
    { 0, 0, 0, 0, 0, 19,0}, //ADC0_SE19_DM0
    { 0, 0, 0, 0, 0, 20,0}, //ADC0_SE20_DM1
    { 0, 0, 0, 0, 0, 26,0}, //ADC0_SE26_TEMP
    { 1, 0, 0, 0, 0, 0, 0}, //ADC1_SE0_DP0
    { 1, 0, 0, 0, 0, 1, 0}, //ADC1_SE1_DP1
    { 1, 0, 0, 0, 0, 3, 0}, //ADC1_SE3_DP3
    { 1, 4, 0, 0, 1, 4, 0}, //ADC1_SE4_PE0
    { 1, 4, 0, 1, 1, 5, 0}, //ADC1_SE5_PE1
    { 1, 4, 0, 2, 1, 6, 0}, //ADC1_SE6_PE2
    { 1, 4, 0, 3, 1, 7, 0}, //ADC1_SE7_PE3
    { 1, 2, 0, 8, 1, 4, 1}, //ADC1_SE4B_PC8
    { 1, 2, 0, 9, 1, 5, 1}, //ADC1_SE5B_PC9
    { 1, 2, 0,10, 1, 6, 1}, //ADC1_SE6B_PC10
    { 1, 2, 0,11, 1, 7, 1}, //ADC1_SE7B_PC11
    { 1, 1, 0, 0, 1, 8, 0}, //ADC1_SE8_PB0
    { 1, 1, 0, 1, 1, 9, 0}, //ADC1_SE9_PB1
    { 1, 1, 0,10, 1, 14,0}, //ADC1_SE14_PB10
    { 1, 1, 0,11, 1, 15,0}, //ADC1_SE15_PB11
    { 1, 0, 0,17, 1, 17,0}, //ADC1_SE17_PA17
    { 1, 0, 0, 0, 0, 19,0}, //ADC1_SE19_DM0
    { 1, 0, 0, 0, 0, 20,0}, //ADC1_SE20_DM1
    { 1, 0, 0, 0, 0, 26,0}, //ADC1_SE26_TEMP
};
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
  //  CalConst(UART_QuickInitTable, ARRAY_SIZE(UART_QuickInitTable));
    while(1)
    {
        shell_main_loop("SHELL>>");
        DelayMs(500);
        UART_printf("I am CHK\r\n");
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
