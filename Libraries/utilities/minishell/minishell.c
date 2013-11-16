/**
  ******************************************************************************
  * @file    minishell.c
  * @author  YANDLD
  * @version V1.0
  * @date    2013.11.25
  * @brief   minishell components
  ******************************************************************************
  */
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "minishell.h"

//! @defgroup MiniShell
//! @{

//! @brief Definitions
#define CB_SIZE			64
#define MAX_ARGS		5
#define MAX_FUNCTION_NUM  (64)

//! @brief global buffer of console input
char gConsoleBuffer[CB_SIZE];
//! @brief prototypes
static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static int CommandFun_Help(int argc, char *argv[]);
//! @brief consts
static const char erase_seq[] = "\b \b";		/* erase sequence	*/
static const char   tab_seq[] = "        ";		/* used to expand TABs	*/

//! @brief Variables
static MINISHELL_InstallTypeDef gInstall;
static MINISHELL_CommandTableTypeDef* gpCmdTable[MAX_FUNCTION_NUM];

//! @brief SHELL Internal function
static MINISHELL_CommandTableTypeDef SHELL_InFunTable[] =
{
    {
        .name = "help",
        .maxargs = 1,
        .cmd = CommandFun_Help,
        .usage = "help",
    },
};
/**
  * @brief  inset a user function in minishell system
  * @param  pAddress: pointer of minishell install struct
  * @retval code: result state
  *         @arg 0: success
  *         @arg 1: already have the function or name conflict
  *         @arg 2: cannot insert any more function
  */
static int SHELL_InsertFunction(MINISHELL_CommandTableTypeDef* pAddress)
{
    uint32_t i;
	  //check name conflict
		for(i = 0; i < MAX_FUNCTION_NUM; i++)
		{
        if(!strcmp(gpCmdTable[i]->name, pAddress->name))
				{
            return 1;
				}
    }
		//find empty pointer
		for(i = 0; i < MAX_FUNCTION_NUM; i++)
		{
        if(gpCmdTable[i] == NULL)
        {
            gpCmdTable[i] = (MINISHELL_CommandTableTypeDef*) pAddress;
            return 0;
        }
		}
		//function slocket is full
		if(i == MAX_FUNCTION_NUM)
    {
        return 2;
    }
		return -1; //impossible
}

void MINISHELL_Init(void)
{
    uint8_t i;
    for(i = 0;i < ARRAY_SIZE(SHELL_InFunTable); i++)
    {
        SHELL_InsertFunction(&SHELL_InFunTable[i]);
    }
}

void MINISHELL_Install(MINISHELL_InstallTypeDef* pInstall)
{
    if((pInstall->ctrl_getchar == NULL) || (pInstall->ctrl_putchar == NULL))
		{
        return;
		}
    gInstall = *pInstall;
}

static uint8_t SHELL_Getc(void)
{
    return gInstall.ctrl_getchar();
}

static void SHELL_Putc(uint8_t ch)
{
    gInstall.ctrl_putchar(ch);
}

static void printchar(char **str, int c)
{
	//extern void putchar(char c);
	
	if (str) {
		**str = (char)c;
		++(*str);
	}
	else
	{ 
		(void)SHELL_Putc(c);
	}
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[MAX_FUNCTION_NUM];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = (unsigned int)i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = (unsigned int)-i;
	}

	s = print_buf + MAX_FUNCTION_NUM-1;
	*s = '\0';

	while (u) {
		t = (unsigned int)u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = (char)(t + '0');
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

/**
  * @brief  mini shell printf function, just like printf
  * @param  format: varible paramater , string to be puted
  * @retval number of bytes to be send
  */
int MINISHELL_printf(const char *format,...)
{
    va_list args;
    va_start( args, format );
    return print( 0, format, args );
}
/**
  * @brief  mini shell interal function: help
  * @param  argc: number of paramer input
  * @param  argv: param contants
  * @retval None
  */
static int CommandFun_Help(int argc, char *argv[])
{
    uint32_t i = 0;
	  MINISHELL_printf("Available Commands:\r\n");
    //dispaly all installed functions
    for(i = 0; i < MAX_FUNCTION_NUM; i++)
    {
        if(gpCmdTable[i] != NULL)
				{
            MINISHELL_printf("name:%s |",     gpCmdTable[i]->name);
            MINISHELL_printf("usage:%s \r\n", gpCmdTable[i]->usage); 
				}
    }
    return 0;
}

/**
  * @brief  Install a list of user application functions
  * @param  SHELL_CommandTableStruct:pointor if minishell funtction install struct.
  * @param  NumberOfFunction: number of user functions
  * @retval None
  */
void MINISHELL_Register(MINISHELL_CommandTableTypeDef* SHELL_CommandTableStruct, uint16_t NumberOfFunction)
{
    uint32_t i;
    if((SHELL_CommandTableStruct != NULL) && (NumberOfFunction != 0))
		{
        for(i = 0; i < NumberOfFunction; i++)
        {
            SHELL_InsertFunction(&SHELL_CommandTableStruct[i]);
        }
		}
}
/**
  * @brief  uninstall a installed function from name
  * @param  name: function cmd string
  * @retval return states
  *         @arg 0:  unregister function successfully
  *         @arg 1:  no such a function installed
  *         @arg -1: unknown err
  */
int MINISHELL_UnRegister(const char* name)
{
    uint32_t i;
    for(i = 0; i < MAX_FUNCTION_NUM; i++)
    {
        //match and unregister
        if(!strcmp(name, gpCmdTable[i]->name))
				{
            gpCmdTable[i] = NULL;
            return 0;
				}
    }
    if(i == MAX_FUNCTION_NUM)
		{
        return 1;
		}
		return -1;
}


static int readline_into_buffer(const char *const prompt, char * buffer)
{
    char *p = buffer;
    char * p_buf = p;
    int	n = 0;				// buffer index
    int	plen = 0;			// prompt length
    int	col;				  // output column cnt
    char	c;

    //printf prompt
    if (prompt)
    {
        plen = strlen(prompt);
        MINISHELL_printf(prompt);
    }
    col = plen;
    for (;;) 
    {
        c = SHELL_Getc();
        //Special character handling
        switch (c) 
        {
            case '\r':				// Enter
            case '\n':        // NewLine
                *p = '\0';
                MINISHELL_printf("\r\n");
                return (p - p_buf);

            case '\0':				// nul
                continue;

            case 0x03:				    // ^C - break	
                p_buf[0] = '\0';	// discard input
                return (-1);

            case 0x15:				    // ^U - erase line
                while (col > plen)
                {
                    MINISHELL_printf(erase_seq);
                    --col;
                }
                p = p_buf;
                n = 0;
                continue;

            case 0x17:				    // ^W - erase word
                p = delete_char(p_buf, p, &col, &n, plen);
                while ((n > 0) && (*p != ' '))
                {
                    p = delete_char(p_buf, p, &col, &n, plen);
                }
                continue;

            case 0x08:				/* ^H  - backspace	*/
            case 0x7F:				/* DEL - backspace	*/
                p = delete_char(p_buf, p, &col, &n, plen);
                continue;

            default:
            //must be a normal character then 
            if (n < CB_SIZE - 2)
            {
                if (c == '\t') 
                {	/* expand TABs		*/
                    MINISHELL_printf(tab_seq + (col & 07));
                    col += 8 - (col & 07);
                }
					    	else 
					    	{
                    ++col;		/* echo input		*/
                    SHELL_Putc(c);
                }
                *p++ = c;
                ++n;
            } 
						else 
						{			/* Buffer full		*/
                SHELL_Putc('\a');
            }
        }
    }
}

static char *delete_char(char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;

    if (*np == 0) 
    {
        return (p);
    }

    if (*(--p) == '\t') 
    {			/* will retype the whole line	*/
        while (*colp > plen) 
        {
            MINISHELL_printf(erase_seq);
            (*colp)--;
        }
        for (s = buffer; s < p; ++s) 
				{
            if (*s == '\t')
            {
                MINISHELL_printf(tab_seq+((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } 
            else
            {
                ++(*colp);
                SHELL_Putc(*s);
            }
        }
    } 
    else 
    {
        MINISHELL_printf(erase_seq);
        (*colp)--;
    }
    (*np)--;

    return (p);
}

static int parse_line(char *line, char *argv[])
{
    int nargs = 0;

#ifdef DEBUG_PARSER
	MINISHELL_printf ("parse_line: \"%s\"\n", line);
#endif
    while (nargs < MAX_ARGS)
    {
    
        /* skip any white space */
        while ((*line == ' ') || (*line == '\t')) 
        {
            ++line;
        }

        if (*line == '\0')
        {	/* end of line, no more args	*/
            argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		MINISHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
            return (nargs);
        }
     
        argv[nargs++] = line;	/* begin of argument string	*/

        /* find end of string */
        while (*line && (*line != ' ') && (*line != '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        {	/* end of line, no more args	*/
            argv[nargs] = NULL;
#ifdef DEBUG_PARSER
    MINISHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
            return (nargs);
        }
        *line++ = '\0';		/* terminate current arg	 */
    }
    MINISHELL_printf ("** Too many args (max. %d) **\n", MAX_ARGS);
#ifdef DEBUG_PARSER
    MINISHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
    return (nargs);
}
/**
  * @brief  find a command form global function solcket
  * @param  cmd: input cmd name
  * @param  cmd: pointer of minishell install struct
  * @param  table_len: number of install struct
  * @retval 
  *         @arg pointer: pointer of mini shell install struct which match the input function name
  *         @arg NULL: no match
  */
static  MINISHELL_CommandTableTypeDef *find_cmd(const char *cmd, MINISHELL_CommandTableTypeDef *table, int table_len)
{
    MINISHELL_CommandTableTypeDef *cmdtp;
    MINISHELL_CommandTableTypeDef *cmdtp_temp = table;	// Init value
    int n_found = 0;

    for (cmdtp = table; cmdtp != table + table_len; cmdtp++)
    {
        if (strncmp(cmd, cmdtp->name, strlen(cmd)) == 0)
        {
            cmdtp_temp = cmdtp;	// abbreviated command ?
            n_found++;
        }
    }
    if (n_found == 1)			// exactly one match
    return cmdtp_temp;
    return NULL;	// not found or ambiguous command
}

/****************************************************************************
 * returns:
 *	1  - command executed, repeatable
 *	0  - command executed but not repeatable, interrupted commands are
 *	     always considered not repeatable
 *	-1 - not executed (unrecognized, bootd recursion or too many args)
 *           (If cmd is NULL or "" or longer than CONFIG_SYS_CBSIZE-1 it is
 *           considered unrecognized)
 *
 * WARNING:
 *
 * We must create a temporary copy of the command since the command we get
 * may be the result from getenv(), which returns a pointer directly to
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */
int run_command(const char *cmd, int flag)
{
    uint8_t i;
	  uint8_t IsMatch = 0;
    MINISHELL_CommandTableTypeDef *cmdtp;
    char *argv[MAX_ARGS + 1];	/* NULL terminated	*/
    int argc;
    if (!cmd || !*cmd)
    {
        return -1;	// empty command
    }
    if (strlen(cmd) >= CB_SIZE)
    {
        MINISHELL_printf("## Command too long!\n");
        return -1;
    }
    // Extract arguments
    if ((argc = parse_line((char *)cmd, argv)) == 0)
    {
        return -1;
    }
    // Look up command in command table
    for(i = 0; i < MAX_FUNCTION_NUM; i++)
		{
        if(find_cmd(argv[0], gpCmdTable[i], 1) != NULL)
        {
            cmdtp = gpCmdTable[i];
            IsMatch = 1;
            break;
        }
		}
    if(IsMatch == 0)
    {
        MINISHELL_printf("Unknown command '%s' - try 'help'\r\n", argv[0]);
        return -1;	
		}
    // found - check max args
    if (argc > cmdtp->maxargs) 
    {
        return -1;
    }
    // OK - call function to do the command
    if ((cmdtp->cmd)(argc, argv) != 0)
    {
        return -1;
    }
    return 0;
}
/**
  * @brief  user API minishell command handle
  * @param  name: host name string
  * @retval none.
  */
void MINISHELL_CmdHandleLoop(char *name)
{
    static char lastcommand[CB_SIZE] = { 0 };
    int len;
    int rc = 1;
    int flag;

    for (;;) 
    {
        len = readline_into_buffer(name, gConsoleBuffer);
        flag = 0;	// assume no special flags for now
        if (len > 0)
        strcpy(lastcommand, gConsoleBuffer);
        if (len == -1)
            MINISHELL_printf("<INTERRUPT>\n");
        else
            rc = run_command(lastcommand, flag);
        if (rc <= 0)
        {
            // invalid command or not repeatable, forget it
            lastcommand[0] = 0;
        }
    }
}

//! @}

