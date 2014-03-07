#include "clock.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "cpuidy.h"
#include "stdio.h" 
#include "common.h"

#include "shell.h"
#include "board.h"

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
extern const cmd_tbl_t CommandFun_SRAM;
extern const cmd_tbl_t CommandFun_LCD;
extern const cmd_tbl_t CommandFun_Hist;
extern const cmd_tbl_t CommandFun_I2C;
extern const cmd_tbl_t CommandFun_KEY;
extern const cmd_tbl_t CommandFun_BUZZER;

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

static shell_io_install_t Shell_IOInstallStruct1 = 
{
	.sh_getc = Getc,
	.sh_putc = Putc,
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

static const QuickInit_Type FTM_QD_QuickInitTable[] = 
{
    { 1, 0, 6,  8, 2, 0}, //FTM1_QD_PHA_PA08_PHB_PA09 6
    { 1, 0, 7, 12, 2, 0}, //FTM1_QD_PHA_PA12_PHB_PA13  7
    { 1, 1, 6,  0, 2, 0}, //FTM1_QD_PHA_PB00_PHB_PB01  6
    { 2, 0, 6, 10, 2, 0}, //FTM2_QD_PHA_PA10_PHB_PA11  6
    { 2, 1, 6, 18, 2, 0}, //FTM2_QD_PHA_PB18_PHB_PB19 6
};

int main(void)
{
    uint32_t i;
    DelayInit();
    UART_Instance = UART_QuickInit(BOARD_UART_DEBUG_MAP,115200);
    printf("HelloWorld\r\n");
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(14);
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
    shell_register_function(&CommandFun_SRAM); 
    shell_register_function(&CommandFun_LCD); 
    shell_register_function(&CommandFun_Hist);
    shell_register_function(&CommandFun_I2C);
    shell_register_function(&CommandFun_KEY);
    shell_register_function(&CommandFun_BUZZER);

    CalConst(FTM_QD_QuickInitTable, ARRAY_SIZE(FTM_QD_QuickInitTable));
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}

void NMI_Handler(void)
{
    printf("NMI Enter\r\n");
}

#ifdef USE_FULL_ASSERT
void assert_failed(char * file, uint32_t line)
{
	//∂œ—‘ ß∞‹ºÏ≤‚
	while(1);
}
#endif
