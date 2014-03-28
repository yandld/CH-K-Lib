#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "cpuidy.h"
#include "stdio.h" 
#include "common.h"

#include "shell.h"
#include "board.h"

#include "GUI.h"
#include "ads7843.h"
#include "spi.h"

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
extern const cmd_tbl_t CommandFun_MP;
extern const cmd_tbl_t CommandFun_LPTMR;
extern const cmd_tbl_t CommandFun_SPI;
extern const cmd_tbl_t CommandFun_DMA;
extern const cmd_tbl_t CommandFun_OV7620;
extern const cmd_tbl_t CommandFun_WDOG;
extern const cmd_tbl_t CommandFun_RTC;
extern const cmd_tbl_t CommandFun_ENET;
extern const cmd_tbl_t CommandFun_TSI;

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
struct spi_bus bus; 
struct ads7843_device ads7843;

int main(void)
{
    uint32_t i;
    DelayInit();
    UART_Instance = UART_QuickInit(BOARD_UART_DEBUG_MAP,115200);
    printf("HelloWorld\r\n");
    /* 设置Flexbus 速度 速度太快ili9320LCD读点有问题 */
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(4);
    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_GPIO); //GPIO命令 测试GPIO
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
    shell_register_function(&CommandFun_MP);
    shell_register_function(&CommandFun_LPTMR);
    shell_register_function(&CommandFun_SPI);
    shell_register_function(&CommandFun_DMA);
    shell_register_function(&CommandFun_OV7620);
    shell_register_function(&CommandFun_WDOG);
    shell_register_function(&CommandFun_OV7620);
    shell_register_function(&CommandFun_RTC);
    shell_register_function(&CommandFun_ENET);
    shell_register_function(&CommandFun_TSI);
    

    spi_bus_init(&bus, BOARD_SPI_INSTANCE);
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    PORT_PinMuxConfig(HW_GPIOD, 14, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOD, 13, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOD, 12, kPinAlt2); 
    

    ads7843.bus = &bus;
    ads7843_init(&ads7843, BOARD_TP_SPI_PCSN, HW_CTAR0, 20*1000);
    ads7843.probe(&ads7843);
    CMD_FLEXBUS(0, NULL);
    if(SRAM_SelfTest())
    {
        printf("SRAM failed\r\n");
        while(1);
    }

  //  GUI_Init();//初始化GUI 
    SYSTICK_Init(1000);
    SYSTICK_ITConfig(ENABLE);
    SYSTICK_Cmd(ENABLE);
	//GUI_DispString("Hello emWin!");//显示测试  
  //  GUIDEMO_Main();
    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}


