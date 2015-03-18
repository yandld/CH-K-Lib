#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "chsw.h"


int main(void)
{   
    uint32_t tmp = 0;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);

    CHSW_IOInit();

    if (!JTAG2SWD()) {
        printf("faile!\r\n");
        return 0;
    }
    uint32_t val;
    swd_read_idcode(&val);
    printf("DP_CTRL_STAT:0x%X\r\n", val);
    
    if (!swd_write_dp(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR)) {
        return 0;
    }

    // Ensure CTRL/STAT register selected in DPBANKSEL
    if (!swd_write_dp(DP_SELECT, 0)) {
        return 0;
    }

    // Power up
    if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
        return 0;
    }

    do {
        if (!swd_read_dp(DP_CTRL_STAT, &tmp)) {
            return 0;
        }
    } while ((tmp & (CDBGPWRUPACK | CSYSPWRUPACK)) != (CDBGPWRUPACK | CSYSPWRUPACK));

    if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE)) {
        return 0;
    }


    if (!swd_read_ap(MDM_IDR, &val)) {
        return 0;
    }
    printf("val:0x%X\r\n", val);
    


    while(1)
    {
       // shell_main_loop("SHELL");
        /* …¡À∏–°µ∆ */
      //  GPIO_ToggleBit(HW_GPIOE, 6);
      //  DelayMs(500);
    }
}










#if 0

#define SHELL_MAX_ARGS      (5)
#define CMD_BUF_LEN         (128)
#include <ctype.h>

typedef struct sh_cmd *sh_cmd_t;

struct sh_cmd
{
    int (*fun)(int argc, int *argv[]);  /* ????, ?????????? */
    char       *name;  /* ????? */
    char       *help;   /* ??????? */
};

static int parse_line (char * line, char * argv[])
{
    uint8_t nargs = 0;
    while (nargs < SHELL_MAX_ARGS) 
    {
        /* skip any white space */
        while (isblank(*line))
        {
            ++line;
        }
        if (*line == '\0')
        {	/* end of line, no more args	*/
            argv[nargs] = NULL;
            return nargs;
        }
        argv[nargs++] = line;	/* begin of argument string	*/
        /* find end of string */
        while ((*line) && (!isblank(*line)))
        {
            ++line;
        }
        if (*line == '\0') 
        {	/* end of line, no more args	*/
            argv[nargs] = NULL;
            return nargs;
        }
        *line++ = '\0';		/* terminate current arg	 */
    }
    printf ("** Too many args (max. %d) **\r\n", SHELL_MAX_ARGS);
    return (nargs);
}




#define ESC         0x1b
#define ENTER       0x0d
#define BACKSPACE   0x08
#define SPACE       0x20

void get_line(void)
{
    char c;
    while(1)
    {
        c = getc();
        if(c == ENTER)
        {
            break;
        }
    }
}

void shell_main_loop(char* prompt)
{
    char cmd_buf[CMD_BUF_LEN];
    char c;
    while(1)
    {
        c = getc();
        printf("%c", c);
    }
}


#endif

