/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdarg.h>
#include "shell.h"

#if __ICCARM__
#include <yfuns.h>
#endif

/*******************************************************************************
 * Defination
 ******************************************************************************/
 
#define CTL_CH(c)		((c) - 'a' + 1)
#define CTL_BACKSPACE		('\b')
#define DEL       ((char)255)
#define DEL7			((char)127)
#define CREAD_HIST_CHAR		('!')
#define putstr(str)  do {while (* str != '\0') {sputc(*str++);}} while (0)
#define sgetc()      gpIOInstallStruct->getc()
#define sputc(ch)    gpIOInstallStruct->putc(ch)

#define BEGINNING_OF_LINE() {			\
	while (num) {				\
		sputc(CTL_BACKSPACE);	\
		num--;				\
	}					\
}

#define ERASE_TO_EOL() {				\
	if (num < eol_num) {				\
		shell_printf("%*s", (int)(eol_num - num), ""); \
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

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
 
extern int cmd_auto_complete(const char * const prompt, char * buf, uint8_t * np, uint8_t * colp);
static void putnstr(char * str, uint8_t n);
/********************************************************************************
 * Variables
 ******************************************************************************/
 
static shell_io_install_t * gpIOInstallStruct;   /* install struct	*/
static const cmd_tbl_t * gpCmdTable[SHELL_MAX_FUNCTION_NUM];  /* cmd table array pointer	*/
static char console_buffer[SHELL_CB_SIZE + 1];	/* console I/O buffer	*/
/* hist support */
#ifdef SHELL_CONFIG_USE_HIST
static int8_t hist_max;
static int8_t hist_add_idx;
static int8_t hist_cur = -1;
static int8_t hist_num;
/* hist buffer */
static char *hist_list[HIST_MAX];
static char hist_lines[HIST_MAX][HIST_SIZE + 1];	/* Save room for NULL */
static char *hist_list[HIST_MAX];
static char hist_lines[HIST_MAX][HIST_SIZE + 1];	/* Save room for NULL */
#endif /* SHELL_CONFIG_USE_HIST */
 /*******************************************************************************
 * Code
 ******************************************************************************/
 
#ifdef SHELL_CONFIG_USE_STDIO

#ifdef __CC_ARM /* MDK Support */
#pragma weak __stdout
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef¡¯ d in stdio.h. */ 
FILE __stdout; 

#pragma weak fputc
int fputc(int ch,FILE *f)
{
	sputc(ch);
	return ch;
}
#pragma weak fgetc
int fgetc(FILE *f)
{
    return sgetc();
}
#elif __ICCARM__/* IAR Support */

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        /* This means that we should flush internal buffers.  Since we*/
        /* don't we just return.  (Remember, "handle" == -1 means that all*/
        /* handles should be flushed.)*/
        return 0;
    }
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != _LLIO_STDOUT) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* Send data.*/
    while (size--)
    {
        sputc(*buffer++);
        ++nChars;
    }
    return nChars;
}

#elif CW/* CodeWarrior Support */
/* It is not supported currently */
    #error "It is not supported in this version!"


#elif __GNUC__/* GCC Support */
int _write (int handle, char * buffer, int size)
{
    int nChars = 0;
    if (buffer == 0)
    {
        /* return -1 if error */
        return -1;
    }
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != 1) && (handle != 2))
    {
        return -1;
    }
    /* Send data.*/
    while (size--)
    {
        sputc(*buffer++);
        ++nChars;
    }
    return nChars;
}

#endif

#else
 /*!
 * @brief use vsprintf for format.
 */
int shell_printf(const char * format,...)
{
    int chars;
    va_list ap;
    char printbuffer[SHELL_CB_SIZE];
    va_start(ap, format);
    chars = vsnprintf(printbuffer, SHELL_CB_SIZE, format, ap);
    //va_end(ap); /* follow MISRA... */
    putnstr(printbuffer, chars);
    return chars ;
}
#endif

 /*!
 * @brief install shell io function.
 */
uint8_t shell_io_install(shell_io_install_t * IOInstallStruct)
{
    uint8_t i;
    gpIOInstallStruct = IOInstallStruct;
    for (i = 0; i < SHELL_MAX_FUNCTION_NUM; i++)
    {
      gpCmdTable[i] = NULL;
    }
    return 0;
}
 /*!
 * @brief beep consult
 */
void shell_beep(void)
{
    sputc('\a');
}
 /*!
 * @brief get global cmdstruct table address
 */
const cmd_tbl_t ** shell_get_cmd_tbl(void)
{
    return gpCmdTable;
}
 /*!
 * @brief send n chars
 */
static void putnstr(char * str, uint8_t n)
{
    while (n--)
    {
        sputc(*str++); 
    }

}
 /*!
 * @brief register a user function
 */
uint8_t shell_register_function(const cmd_tbl_t * pAddress)
{
    uint32_t i = 0;
    /* check name conflict */
    while ((gpCmdTable[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
    {
        if (!strcmp(gpCmdTable[i]->name, pAddress->name))
        {
            return 1;
        }
        i++;
    }
    /* insert */
    for (i = 0; i< SHELL_MAX_FUNCTION_NUM; i++)
    {
        if (gpCmdTable[i] == NULL)
        {
            gpCmdTable[i] =  pAddress;
            return 0;
        }
    }
    return 1;
}
 /*!
 * @brief register function array
 */
void shell_register_function_array(const cmd_tbl_t * pAddress, uint8_t num)
{
    const cmd_tbl_t * cmdtp =  pAddress;
    while (num--)
    {
        shell_register_function(cmdtp++);
    }
}

 /*!
 * @brief unregister function
 */
uint8_t shell_unregister_function(char * name)
{
    uint8_t i,j;
    i = 0;
    while ((gpCmdTable[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
    {
        if (!strcmp(name, gpCmdTable[i]->name))
        {
            j = i + 1;
            while (gpCmdTable[j] != NULL)
            {
                gpCmdTable[j-1] = gpCmdTable[j];
                j++;
            }
            gpCmdTable[j-1] = NULL;
        }
        i++;
    }
    return 0;
}
#ifdef SHELL_CONFIG_USE_HIST
 /*!
 * @brief init history buffer and vars
 */
static void hist_init(void)
{
    uint8_t i;
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

 /*!
 * @brief and line string to history buffer
 */
static void cread_add_to_hist(char * line)
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

 /*!
 * @brief get history data list and also get number of the list
 */
char ** shell_get_hist_data_list(uint8_t * num)
{
    *num = hist_max;
    return  &hist_list[0];
}

 /*!
 * @brief return previous history string
 */
static char * hist_prev(void)
{
    char * ret;
    uint8_t old_cur;
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

 /*!
 * @brief return next history string
 */
static char * hist_next(void)
{
    char * ret;
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
#endif
 /*!
 * @brief add a char to conslt buffer
 */
static void cread_add_char(char ichar, uint8_t insert, uint8_t * num,
	       uint8_t * eol_num, char * buf, uint8_t len)
{
    unsigned long wlen;

    /* room ??? */
    if (insert || (*num == *eol_num))
    {
        if (*eol_num > len - 1)
        {
            shell_beep();
            return;
        }
        (*eol_num)++;
    }
    if (insert)
    {
        wlen = *eol_num - *num;
        if (wlen > 1)
        {
            memmove(&buf[*num+1], &buf[*num], wlen-1);
        }
        buf[*num] = ichar;
        putnstr(buf + *num, wlen);
        (*num)++;
        while (--wlen)
        {
            sputc(CTL_BACKSPACE);
        }
    }
    else
    {
        /* echo the character */
        wlen = 1;
        buf[*num] = ichar;
        putnstr(buf + *num, wlen);
        (*num)++;
    }
}

 /*!
 * @brief add string to conslt buffer
 */
static void cread_add_str(char * str, uint8_t strsize, uint8_t insert, uint8_t * num,
	      uint8_t * eol_num, char * buf, uint8_t len)
{
    while (strsize--)
    {
        cread_add_char(*str, insert, num, eol_num, buf, len);
        str++;
    }
}

 /*!
 * @brief read line into buffer
 */
static int cread_line(const char * const prompt, char * buf, uint8_t * len)
{
    uint8_t num = 0;
    uint8_t eol_num = 0;
    uint8_t wlen;
    char ichar = 0;
    uint8_t insert = 1;
    uint8_t esc_len = 0;
    char esc_save[8];
    uint8_t init_len = strlen(buf);
    if (init_len)
    {
        cread_add_str(buf, init_len, 1, &num, &eol_num, buf, *len);
    }
    while (1) 
    {
        do
        {
            ichar = sgetc();
        }while(!ichar);
        if ((ichar == '\n') || (ichar == '\r')) 
        {
            sputc('\r');sputc('\n');
            break;
        }
        /* handle standard linux xterm esc sequences for arrow key, etc.*/
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
                shell_printf("impossible condition #876\n");
                esc_len = 0;
            }
            break;
        case CTL_CH('a'):
            BEGINNING_OF_LINE()
            break;
        case CTL_CH('c'):	/* ^C - break */
            *buf = '\0';	/* discard input */
            return (-1);
            break; /* have to follow MISRA */
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
            ERASE_TO_EOL()
            break;
        case CTL_CH('e'):
            REFRESH_TO_EOL()
            break;
        case CTL_CH('o'):
            insert = !insert;
            break;
        case CTL_CH('x'):
        case CTL_CH('u'):
            BEGINNING_OF_LINE()
            ERASE_TO_EOL()
            break;
        case DEL:
        case DEL7:
        case 8:
            if (num)
            {
                wlen = eol_num - num;
                num--;
                memmove(&buf[num], &buf[num+1], wlen);
                sputc(CTL_BACKSPACE);
                putnstr(buf + num, wlen);
                sputc(' ');
                do
                {
                    sputc(CTL_BACKSPACE);
                } while (wlen--);
                eol_num--;
            }
            break;

        case CTL_CH('p'):
        case CTL_CH('n'):
#ifdef SHELL_CONFIG_USE_HIST
        {
            char * hline;
            esc_len = 0;
            if (ichar == CTL_CH('p'))
            {
                hline = hist_prev();
            }
            else
            {
                hline = hist_next();
            }
            if (!hline)
            {
                shell_beep();
                continue;
            }
            /* nuke the current line */
            /* first, go home */
            BEGINNING_OF_LINE()
            ERASE_TO_EOL()
            /* copy new line into place and display */
            strcpy(buf, hline);
            eol_num = strlen(buf);
            REFRESH_TO_EOL()
            continue;
            break; /* have to follow MISRA */
        }
#else
        {
            continue;
        }
        
#endif /* SHELL_CONFIG_USE_HIST */
        case '\t': 
#ifdef SHELL_CONFIG_AUTO_COMPLETE
        {
            uint8_t num2, col;
            /* do not autocomplete when in the middle */
            if (num < eol_num)
            {
                shell_beep();
                break;
            }
            buf[num] = '\0';
            col = strlen(prompt) + eol_num;
            num2 = num;
            if (cmd_auto_complete(prompt, buf, &num2, &col))
            {
                col = num2 - num;
                num += col;
                eol_num += col;
            }
            break;
        }
#else
        {
            continue;
        }
#endif /* SHELL_CONFIG_AUTO_COMPLETE */
        default:
            cread_add_char(ichar, insert, &num, &eol_num, buf, *len);
            break;
        }
    }
    *len = eol_num;
    buf[eol_num] = '\0';	/* lose the newline */
#ifdef SHELL_CONFIG_USE_HIST
    if ((buf[0]) && (buf[0] != CREAD_HIST_CHAR))
    {
        cread_add_to_hist(buf);
    }
    hist_cur = hist_add_idx;
#endif
    return 0;
}

 /*!
 * @brief read line from consult
 */
static int readline (char * prompt)
{
    uint8_t len = SHELL_CB_SIZE;
    int8_t rc;
    char * p = console_buffer;
    static uint8_t initted = 0;
    /* break console_buffer so that is will not repeatable */
    console_buffer[0] = '\0';
    if (!initted)
    {
#ifdef SHELL_CONFIG_USE_HIST
        hist_init();
#endif
        initted = 1;
        shell_printf("\r\nSHELL (build: %s)\r\n", __DATE__);
        shell_printf("Copyright (c) 2013 Freescale Semiconductor\r\n");
    }
    if (prompt)
    {
        putstr (prompt);
    }
    rc = cread_line(prompt, p, &len);
    return rc < 0 ? rc : len;
}

 /*!
 * @brief extract from readline
 */
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
    shell_printf ("** Too many args (max. %d) **\r\n", SHELL_MAX_ARGS);
    return (nargs);
}

 /*!
 * @brief find command form command struct's name
 */
const cmd_tbl_t * shell_find_command (const char * cmd)
{
    uint8_t i = 0, n_found = 0;
    const cmd_tbl_t *cmdtp_temp = NULL;
    if (cmd == NULL) 
    {
        return NULL;
    }
    while ((gpCmdTable[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
    {
        if (!strcmp(cmd, gpCmdTable[i]->name))
        {
            cmdtp_temp = gpCmdTable[i];
            n_found++;
        }
        i++;
    }
    /* we need exactly one match */
    if (n_found == 1)
    {
        return cmdtp_temp;
    }
    return NULL;
}

void shell_main_loop(char * prompt)
{
    int8_t len;
    uint8_t argc;
    int8_t result;
    const cmd_tbl_t * cmdtp;
    char * argv[SHELL_MAX_ARGS];	/* NULL terminated	*/
    for (;;)
    {
        len = readline(prompt);
        if (len > 0)
        {
            if ((argc = parse_line (console_buffer, argv)) == 0) 
            {
                continue;
            }
            cmdtp = shell_find_command(argv[0]);
            if ((cmdtp != NULL) && (cmdtp->cmd != NULL))
            {
                if (argc > cmdtp->maxargs)
                {
                    result = CMD_RET_USAGE;
                }
                else
                {
                    result = (cmdtp->cmd)(argc, argv);
                }
            }
            else
            {
                shell_printf("Unknown command '%s' - try 'help'\r\n", argv[0]);	
            }
            if (result == CMD_RET_USAGE)
            {
                if (cmdtp->usage != NULL)
                {
                    shell_printf("%s - %s\r\n", cmdtp->name, cmdtp->usage);
                }
                if (cmdtp->help != NULL)
                {
                    shell_printf("Usage:\r\n%s ", cmdtp->name);
                    shell_printf("%s\r\n", cmdtp->help);
                }
                else
                {
                    shell_printf ("- No additional help available.\r\n");
                }
            }
        }
        else if (len == -1)
        {
            shell_printf("<INTERRUPT>\r\n");
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
 
 
