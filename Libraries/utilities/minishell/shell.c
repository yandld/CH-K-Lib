#include "shell.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define CB_SIZE			64
#define MAX_ARGS		5
char console_buffer[CB_SIZE];		/* console I/O buffer	*/
static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static const char erase_seq[] = "\b \b";		/* erase sequence	*/
static const char   tab_seq[] = "        ";		/* used to expand TABs	*/
static SHELL_CommandTableTypeDef* SHELL_pCmdTable = NULL;
static uint16_t SHELL_ExFunNum = 0;


static uint8_t SHELL_Getc(void)
{
    uint8_t ch;
    UART_ReceiveByte(UART_DebugPort, &ch);
    return ch;
}

//! @brief send a char via uart
static void SHELL_Putc(uint8_t ch)
{
    UART_SendByte(UART_DebugPort,ch);
}

int SHELL_printf(const char *format,...)
{
    int chars;
    int i;
    va_list ap;
    char printbuffer[CB_SIZE];
    va_start(ap, format);
    chars = vsprintf(printbuffer, format, ap);
    va_end(ap);
    for(i=0;i<chars;i++)
    {
        SHELL_Putc(printbuffer[i]);
    }
    return chars ;
}

//SHELL Internal function
static int CommandFun_Help(int argc, char *argv[]);

SHELL_CommandTableTypeDef sf_cmd_tbl[] =
{
    { "help", 1, CommandFun_Help ,"help" },
    { "list", 1, CommandFun_Help ,"help" },
};

static int CommandFun_Help(int argc, char *argv[])
{
    uint8_t i = 0;
    //dispaly internal functions
    for(i = 0; i < ARRAY_SIZE(sf_cmd_tbl) ;i++)
    {
        SHELL_printf("name:%s |", sf_cmd_tbl[i].name);
        SHELL_printf("usage:%s \r\n", sf_cmd_tbl[i].usage);
    }
		//display extneral functions
		if((SHELL_pCmdTable != NULL) && (SHELL_ExFunNum != 0))
		{
        for(i = 0; i < SHELL_ExFunNum; i++)
        {
            SHELL_printf("name:%s |", SHELL_pCmdTable[i].name);
            SHELL_printf("usage:%s \r\n", SHELL_pCmdTable[i].usage);     
        }
		}
}



//! @brief MiniShell register user defined functions
void SHELL_RegisterFunction(SHELL_CommandTableTypeDef* SHELL_CommandTableStruct, uint16_t NumberOfFunction)
{
    SHELL_pCmdTable = SHELL_CommandTableStruct;
    SHELL_ExFunNum = NumberOfFunction;
}

static int readline_into_buffer(const char *const prompt, char * buffer)
{
    char *p = buffer;
    char * p_buf = p;
    int	n = 0;				/* buffer index		*/
    int	plen = 0;			/* prompt length	*/
    int	col;				/* output column cnt	*/
    char	c;

    //printf prompt
    if (prompt)
    {
        plen = strlen(prompt);
        SHELL_printf(prompt);
    }
    col = plen;
    for (;;) 
    {
        c = SHELL_Getc();
        //Special character handling
        switch (c) 
        {
            case '\r':				/* Enter		*/
            case '\n':
                *p = '\0';
                SHELL_printf("\r\n");
                return (p - p_buf);

            case '\0':				/* nul			*/
                continue;

            case 0x03:				/* ^C - break		*/
                p_buf[0] = '\0';	/* discard input */
                return (-1);

            case 0x15:				/* ^U - erase line	*/
                while (col > plen)
                {
                    SHELL_printf(erase_seq);
                    --col;
                }
                p = p_buf;
                n = 0;
                continue;

            case 0x17:				/* ^W - erase word	*/
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
                    SHELL_printf(tab_seq + (col & 07));
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
            SHELL_printf(erase_seq);
            (*colp)--;
        }
        for (s = buffer; s < p; ++s) 
				{
            if (*s == '\t')
            {
                SHELL_printf(tab_seq+((*colp) & 07));
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
        SHELL_printf(erase_seq);
        (*colp)--;
    }
    (*np)--;

    return (p);
}


static int parse_line(char *line, char *argv[])
{
	int nargs = 0;

#ifdef DEBUG_PARSER
	SHELL_printf ("parse_line: \"%s\"\n", line);
#endif
	while (nargs < MAX_ARGS) {

		/* skip any white space */
		while ((*line == ' ') || (*line == '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		SHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
			return (nargs);
		}
     
		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && (*line != ' ') && (*line != '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		SHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
			return (nargs);
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	SHELL_printf ("** Too many args (max. %d) **\n", MAX_ARGS);

#ifdef DEBUG_PARSER
	SHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
	return (nargs);
}

static  SHELL_CommandTableTypeDef *find_cmd(const char *cmd, SHELL_CommandTableTypeDef *table, int table_len)
{
	 SHELL_CommandTableTypeDef *cmdtp;
	 SHELL_CommandTableTypeDef *cmdtp_temp = table;	/*Init value */
	int len;
	int n_found = 0;

	for (cmdtp = table;
		cmdtp != table + table_len;
		cmdtp++) {
		if (strncmp(cmd, cmdtp->name, strlen(cmd)) == 0) {
			if (len == strlen(cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1)			/* exactly one match */
		return cmdtp_temp;

	return NULL;	/* not found or ambiguous command */
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
	 SHELL_CommandTableTypeDef *cmdtp;
	char *argv[MAX_ARGS + 1];	/* NULL terminated	*/
	int argc;
  char isMatch = TRUE;
#ifdef DEBUG_PARSER
	SHELL_printf ("[RUN_COMMAND] cmd[%p]=\"", cmd);
	SHELL_printf (cmd ? cmd : "NULL");	/* use puts - string may be loooong */
	SHELL_printf ("\"\n");
#endif
   
	if (!cmd || !*cmd) {
		return -1;	/* empty command */
	}

	if (strlen(cmd) >= CB_SIZE) {
		SHELL_printf("## Command too long!\n");
		return -1;
	}

	/* Process separators and check for invalid
	 * repeatable commands
	 */

#ifdef DEBUG_PARSER
	SHELL_printf ("[PROCESS_SEPARATORS] %s\n", cmd);
#endif

	/* Extract arguments */
	if ((argc = parse_line((char *)cmd, argv)) == 0) {
		return -1;
	}
	/* Look up command in command table */
    if ((cmdtp = find_cmd(argv[0], sf_cmd_tbl, (ARRAY_SIZE(sf_cmd_tbl)))) == NULL && ((cmdtp = find_cmd(argv[0], SHELL_pCmdTable, SHELL_ExFunNum)) == NULL)) 
    {
		    SHELL_printf("Unknown command '%s' - try 'help'\r\n", argv[0]);
		    return -1;	
    }

	
		/* found - check max args */
	if (argc > cmdtp->maxargs) {
		return -1;
	}

	/* OK - call function to do the command */
	if ((cmdtp->cmd)(argc, argv) != 0) {
		return -1;
	}
	return 0;
}

void SHELL_CmdHandleLoop(char *name)
{
	static char lastcommand[CB_SIZE] = { 0 };
	int len;
	int rc = 1;
	int flag;

	/*
	 * Main Loop for Monitor Command Processing
	 */
	for (;;) {
		len = readline_into_buffer(name, console_buffer);

		flag = 0;	/* assume no special flags for now */
		if (len > 0)
			strcpy(lastcommand, console_buffer);

		if (len == -1)
			SHELL_printf("<INTERRUPT>\n");
		else
			rc = run_command(lastcommand, flag);

		if (rc <= 0) {
			/* invalid command or not repeatable, forget it */
			lastcommand[0] = 0;
		}
	}
}


