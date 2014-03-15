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
extern const cmd_tbl_t CommandFun_LPTMR;
extern const cmd_tbl_t CommandFun_SPI;
extern const cmd_tbl_t CommandFun_DMA;
extern const cmd_tbl_t CommandFun_OV7620;


static const QuickInit_Type SPI_QuickInitTable[] =
{
    { 0, 2, 2, 5, 3, 0}, //SPI0_SCK_PC05_SOUT_PC06_SIN_PC07 2
    { 0, 3, 2, 1, 3, 0}, //SPI0_SCK_PD01_SOUT_PD02_SIN_PD03 2
    { 1, 4, 2, 1, 3, 0}, //SPI1_SCK_PE02_SOUT_PE01_SIN_PE03 2
    { 0, 0, 2,15, 3, 0}, //SPI0_SCK_PA15_SOUT_PA16_SIN_PA17 2
    { 2, 1, 2,21, 3, 0}, //SPI2_SCK_PB21_SOUT_PB22_SIN_PB23 2
    { 2, 3, 2,12, 3, 0}, //SPI2_SCK_PD12_SOUT_PD13_SIN_PD14 2
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
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
int main(void)
{
    uint32_t i;
    DelayInit();
    UART_Instance = UART_QuickInit(BOARD_UART_DEBUG_MAP,115200);
    printf("HelloWorld\r\n");
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(0);
    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_GPIO); //GPIOÃüÁî ²âÊÔGPIO
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
    shell_register_function(&CommandFun_LPTMR);
    shell_register_function(&CommandFun_SPI);
    shell_register_function(&CommandFun_DMA);
    shell_register_function(&CommandFun_OV7620);
    CalConst(SPI_QuickInitTable, ARRAY_SIZE(SPI_QuickInitTable));
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}

//!< None Maskable Interrupt
void NMI_Handler(void)
{
    printf("NMI Enter\r\n");
}

#ifdef USE_FULL_ASSERT
void assert_failed(char * file, uint32_t line)
{
	//¶ÏÑÔÊ§°Ü¼ì²â
	while(1);
}
#endif
