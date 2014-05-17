#include "gpio.h"
#include "uart.h"
#include "systick.h"
#include "shell.h"


int CMD_GPIO(int argc, char *const argv[])
{
    printf("enter CMD_GPIO function\r\n");
    while(argc--)
    {
        printf("argv:%s\r\n", argv[argc]);
    }
    return 0;
}

int main(void)
{
    DelayInit();
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("HelloWorld\r\n");
    /* 初始化shell之前 一定要实现stdout 和 stdin */
    /* 还需要在 Linker 的 MiscControl中定义： --keep __fsym_* */
    shell_init();

    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}

SHELL_EXPORT_CMD(CMD_GPIO, gpio ,help of GPIO);


