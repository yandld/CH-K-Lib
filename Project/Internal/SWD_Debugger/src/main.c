#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "chsw.h"




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




#define ESC         0x1BU
#define ENTER       0x0DU
#define BACKSPACE   0x7FU
#define SPACE       0x20U

void get_line(char *buf, uint32_t len)
{
    char i;
    
    i = 0;
    memset(buf, len, 0);
    
    while(len--)
    {
        *buf = getc();
        if(*buf == ENTER)
        {
            putc('\r');
            putc('\n');
            break;
        }
        
        switch(*buf)
        {
            case BACKSPACE:
                if(i > 0)
                {
                    len++;
                    *buf--;
                    putc(BACKSPACE);
                    i--;
                }
                break;
            default:
                i++;
                putc(*buf++);
                len--;
                break;
        }
    }
}


void shell_main_loop(char* prompt)
{
    char cmd_buf[CMD_BUF_LEN];

    while(1)
    {
        printf("%s>>", prompt);
        get_line(cmd_buf, CMD_BUF_LEN);
        printf("data:%s\r\n", cmd_buf);
    }
}

int main(void)
{   
    uint32_t tmp, val;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);

    CHSW_IOInit();

    swd_init_debug();
    
    swd_read_idcode(&val);
    printf("DP_IDR:0x%X\r\n", val);
    


    int res;
    
    swd_read_ap(MDM_IDR, &val);
    printf("val:0x%X\r\n", val);
   // res = swd_write_ap(MDM_CTRL, 0x55);
  //  printf("res:%d\r\n", res);
    swd_read_ap(MDM_CTRL, &val);
    printf("val:0x%X\r\n", val);
    


    while(1)
    {
        shell_main_loop("SHELL");
        /* …¡À∏–°µ∆ */
      //  GPIO_ToggleBit(HW_GPIOE, 6);
      //  DelayMs(500);
    }
}










