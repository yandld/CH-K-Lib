#include "shell.h"
#include "shell_autocomplete.h"
#include "common.h"
#include <stdio.h>

static uint8_t SHELL_GetChar(void);
static void SHELL_PutChar(uint8_t ch);

#define CTL_CH(c)		((c) - 'a' + 1)
#define CTL_BACKSPACE		('\b')
#define DEL       ((char)255)
#define DEL7			((char)127)
#define CREAD_HIST_CHAR		('!')


#define sputc(ch)	        SHELL_PutChar(ch)
#define sgetc()		        SHELL_GetChar()
#define getcmd_cbeep()		sputc('\a')



static void putnstr(char* str, uint8_t n)
{
	while(n--) sputc(*str++);
}


void mputs(const char *str)
{
	while(*str != '\0')
	{
		sputc(*str++);
	}
}


SHELL_IOInstall_Type* gpIOInstallStruct;
cmd_tbl_t* gpCmdTable[SHELL_MAX_FUNCTION_NUM];


struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef¡¯ d in stdio.h. */ 
FILE __stdout; 
int fputc(int ch,FILE *f)
{
	SHELL_PutChar(ch);
	return ch;
}

int fgetc(FILE *f)
{
    return SHELL_GetChar();
}


SHELL_Status_Type SHELL_IOInstall(SHELL_IOInstall_Type* IOInstallStruct)
{
    if(IOInstallStruct == NULL)
		{
        return kShell_Fail;
		}
    gpIOInstallStruct = IOInstallStruct;
    return kShell_Success;
}

static uint8_t SHELL_GetChar(void)
{
    return gpIOInstallStruct->getc();
}

static void SHELL_PutChar(uint8_t ch)
{
    gpIOInstallStruct->putc(ch);
}



SHELL_Status_Type SHELL_InsertFunction(cmd_tbl_t* pAddress)
{
    uint32_t i;
	  //check name conflict
		for(i = 0; i < SHELL_MAX_FUNCTION_NUM; i++)
		{
        if(!strcmp(gpCmdTable[i]->name, pAddress->name))
				{
            return kShell_Fail;
				}
    }
		//find empty pointer
		for(i = 0; i < SHELL_MAX_FUNCTION_NUM; i++)
		{
        if(gpCmdTable[i] == NULL)
        {
            gpCmdTable[i] = (cmd_tbl_t*) pAddress;
            return kShell_Success;
        }
		}
		return kShell_Fail; //impossible
}



#define HIST_MAX		20
#define HIST_SIZE		SHELL_CB_SIZE

#define BEGINNING_OF_LINE() {			\
	while (num) {				\
		sputc(CTL_BACKSPACE);	\
		num--;				\
	}					\
}

#define ERASE_TO_EOL() {				\
	if (num < eol_num) {				\
		printf("%*s", (int)(eol_num - num), ""); \
		do {					\
			sputc(CTL_BACKSPACE);	\
		} while (--eol_num > num);		\
	}						\
}

#define REFRESH_TO_EOL() {			\
	if (num < eol_num) {			\
		wlen = eol_num - num;		\
		putnstr(buf + num, wlen);	\
		num = eol_num;			\
	}					\
}

char console_buffer[SHELL_CB_SIZE + 1];	/* console I/O buffer	*/

static int hist_max;
static int hist_add_idx;
static int hist_cur = -1;
static unsigned hist_num;

static char *hist_list[HIST_MAX];
static char hist_lines[HIST_MAX][HIST_SIZE + 1];	/* Save room for NULL */

static void hist_init(void)
{
    int i;

    hist_max = 0;
    hist_add_idx = 0;
    hist_cur = -1;
    hist_num = 0;

    for (i = 0; i < HIST_MAX; i++)
    {
        hist_list[i] = hist_lines[i];
        hist_list[i][0] = '\0';
    }
}


static void cread_add_to_hist(char *line)
{
    strcpy(hist_list[hist_add_idx], line);

    if (++hist_add_idx >= HIST_MAX)
		{
        hist_add_idx = 0;
		}
		
		if (hist_add_idx > hist_max)
		{
        hist_max = hist_add_idx;
		}
		hist_num++;
}

static char* hist_prev(void)
{
    char *ret;
    int old_cur;

    if (hist_cur < 0)
    {
        return NULL;
    }

    old_cur = hist_cur;
    if (--hist_cur < 0)
		{
        hist_cur = hist_max;
		}

		if (hist_cur == hist_add_idx) 
		{
				hist_cur = old_cur;
				ret = NULL;
		} 
		else
		{
        ret = hist_list[hist_cur];
		}
    return (ret);
}

static char* hist_next(void)
{
    char *ret;

    if (hist_cur < 0)
    {
        return NULL;
    }
    if (hist_cur == hist_add_idx)
		{
        return NULL;
		}
		if (++hist_cur > hist_max)
		{
				hist_cur = 0;
		}
		if (hist_cur == hist_add_idx)
		{
				ret = "";
		}
		else
		{
				ret = hist_list[hist_cur];
		}
	return (ret);
}

static void cread_add_char(char ichar, int insert, unsigned long *num,
	       unsigned long *eol_num, char *buf, unsigned long len)
{
	unsigned long wlen;

	/* room ??? */
	if (insert || *num == *eol_num) {
		if (*eol_num > len - 1) {
			getcmd_cbeep();
			return;
		}
		(*eol_num)++;
	}

	if (insert) {
		wlen = *eol_num - *num;
		if (wlen > 1) {
			memmove(&buf[*num+1], &buf[*num], wlen-1);
		}

		buf[*num] = ichar;
		putnstr(buf + *num, wlen);
		(*num)++;
		while (--wlen) {
			sputc(CTL_BACKSPACE);
		}
	} else {
		/* echo the character */
		wlen = 1;
		buf[*num] = ichar;
		putnstr(buf + *num, wlen);
		(*num)++;
	}
}


static void cread_add_str(char *str, int strsize, int insert, unsigned long *num,
	      unsigned long *eol_num, char *buf, unsigned long len)
{
	while (strsize--) {
		cread_add_char(*str, insert, num, eol_num, buf, len);
		str++;
	}
}



static int cread_line(const char *const prompt, char *buf, unsigned int *len, int timeout)
{
    unsigned long num = 0;
    unsigned long eol_num = 0;
    unsigned long wlen;
    char ichar;
    int insert = 1;
    int esc_len = 0;
    char esc_save[8];
    int init_len = strlen(buf);
    int first = 1;

    if (init_len)
    {
        cread_add_str(buf, init_len, 1, &num, &eol_num, buf, *len);
    }
    while (1) 
    {
        ichar = sgetc();
        if ((ichar == '\n') || (ichar == '\r')) 
        {
            sputc('\r');sputc('\n');
            break;
        }
		/*
		 * handle standard linux xterm esc sequences for arrow key, etc.
		 */
        if (esc_len != 0)
        {
            if (esc_len == 1) 
            {
                if (ichar == '[')
                {
                    esc_save[esc_len] = ichar;
                    esc_len = 2;
                } 
                else
                {
                    cread_add_str(esc_save, esc_len, insert, &num, &eol_num, buf, *len);
                    esc_len = 0;
                }
                continue;
            } 

        switch (ichar) 
        {

            case 'D':	/* <- key */
                ichar = CTL_CH('b');
                esc_len = 0;
                break;
            case 'C':	/* -> key */
                ichar = CTL_CH('f');
                esc_len = 0;
                break;	/* pass off to ^F handler */
            case 'H':	/* Home key */
                ichar = CTL_CH('a');
                esc_len = 0;
                break;	/* pass off to ^A handler */
            case 'A':	/* up arrow */
                ichar = CTL_CH('p');
                esc_len = 0;
                break;	/* pass off to ^P handler */
            case 'B':	/* down arrow */
                ichar = CTL_CH('n');
                esc_len = 0;
                break;	/* pass off to ^N handler */
            default:
                esc_save[esc_len++] = ichar;
                cread_add_str(esc_save, esc_len, insert,
					      &num, &eol_num, buf, *len);
                esc_len = 0;
				continue;
			}
		}

    switch (ichar)
    {
        case 0x1b:
            if (esc_len == 0) 
            {
                esc_save[esc_len] = ichar;
                esc_len = 1;
            }
            else 
            {
                mputs("impossible condition #876\n");
                esc_len = 0;
            }
            break;

        case CTL_CH('a'):
            BEGINNING_OF_LINE();
            break;
        case CTL_CH('c'):	/* ^C - break */
            *buf = '\0';	/* discard input */
            return (-1);
        case CTL_CH('f'):
            if (num < eol_num)
            {
                sputc(buf[num]);
                num++;
            }
            break;
        case CTL_CH('b'):
            if (num)
            {
                sputc(CTL_BACKSPACE);
                num--;
            }
            break;
        case CTL_CH('d'):
            if (num < eol_num)
            {
                wlen = eol_num - num - 1;
                if (wlen)
                {
                    memmove(&buf[num], &buf[num+1], wlen);
                    putnstr(buf + num, wlen);
                }

                sputc(' ');
                do 
                {
                    sputc(CTL_BACKSPACE);
                } while (wlen--);
                eol_num--;
            }
            break;
        case CTL_CH('k'):
            ERASE_TO_EOL();
            break;
        case CTL_CH('e'):
            REFRESH_TO_EOL();
            break;
        case CTL_CH('o'):
            insert = !insert;
            break;
        case CTL_CH('x'):
        case CTL_CH('u'):
            BEGINNING_OF_LINE();
            ERASE_TO_EOL();
            break;
		case DEL:
		case DEL7:
		case 8:
			if (num) {
				wlen = eol_num - num;
				num--;
				memmove(&buf[num], &buf[num+1], wlen);
				sputc(CTL_BACKSPACE);
				putnstr(buf + num, wlen);
				sputc(' ');
				do {
					sputc(CTL_BACKSPACE);
				} while (wlen--);
				eol_num--;
			}
			break;
		case CTL_CH('p'):
		case CTL_CH('n'):
		{
			char * hline;

			esc_len = 0;

			if (ichar == CTL_CH('p'))
				hline = hist_prev();
			else
				hline = hist_next();

			if (!hline) {
				getcmd_cbeep();
				continue;
			}

			/* nuke the current line */
			/* first, go home */
			BEGINNING_OF_LINE();

			/* erase to end of line */
			ERASE_TO_EOL();

			/* copy new line into place and display */
			strcpy(buf, hline);
			eol_num = strlen(buf);
			REFRESH_TO_EOL();
			continue;
		}
#ifdef CONFIG_AUTO_COMPLETE
		case '\t': {
			int num2, col;

			/* do not autocomplete when in the middle */
			if (num < eol_num) {
				getcmd_cbeep();
				break;
			}
      
			buf[num] = '\0';
			col = strlen(prompt) + eol_num;
			num2 = num;
			
			if (cmd_auto_complete(prompt, buf, &num2, &col)) {
				col = num2 - num;
				num += col;
				eol_num += col;
			}
			
			break;
			
		}
#else

#endif
		default:
			cread_add_char(ichar, insert, &num, &eol_num, buf, *len);
			break;
		}
	}
	*len = eol_num;
	buf[eol_num] = '\0';	/* lose the newline */

	if (buf[0] && buf[0] != CREAD_HIST_CHAR)
		cread_add_to_hist(buf);
	hist_cur = hist_add_idx;

	return 0;
}



int readline_into_buffer(const char *const prompt, char *buffer, int timeout)
{
	char *p = buffer;
	unsigned int len = SHELL_CB_SIZE;
	int rc;
	static int initted = 0;

	/*
	 * History uses a global array which is not
	 * writable until after relocation to RAM.
	 * Revert to non-history version if still
	 * running from flash.
	 */
		if (!initted) {
			hist_init();
			initted = 1;
		}

		if (prompt)
			mputs (prompt);

		rc = cread_line(prompt, p, &len, timeout);
		return rc < 0 ? rc : len;

}



int readline (const char *const prompt)
{
	/*
	 * If console_buffer isn't 0-length the user will be prompted to modify
	 * it instead of entering it from scratch as desired.
	 */
	console_buffer[0] = '\0';

	return readline_into_buffer(prompt, console_buffer, 0);
}



/****************************************************************************/

int parse_line (char *line, char *argv[])
{
	int nargs = 0;

	while (nargs < 20) {

		/* skip any white space */
		while (isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return nargs;
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && !isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return nargs;
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	printf ("** Too many args (max. %d) **\n", 20);

	return (nargs);
}


void main_loop()
{
	uint8_t len;
	uint8_t rc;
	uint8_t argc;
	uint8_t result;
  cmd_tbl_t *cmdtp;
	char *argv[20];	/* NULL terminated	*/
		for(;;)
		{
	    	len = 	readline("MS>>");
		   if ((argc = parse_line (console_buffer, argv)) == 0) 
				  {
		  	    rc = -1;	/* no command at all */
		      	continue;
	      	}
	      cmdtp = find_cmd(argv[0]);
				if(cmdtp != NULL)
				{
					result = (cmdtp->cmd)(cmdtp, 0, argc, argv);
				}
				else
				{
            printf("Unknown command '%s' - try 'help'\r\n", argv[0]);	
				}
		}
}
