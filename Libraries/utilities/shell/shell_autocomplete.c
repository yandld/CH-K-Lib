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
#include "shell.h"
#include <string.h>


/*******************************************************************************
 * Defination
 ******************************************************************************/



/*******************************************************************************
 * Variables
 ******************************************************************************/
 static char tmp_buf[SHELL_CB_SIZE];	/* copy of console I/O buffer	*/
 
 /*******************************************************************************
 * Code
 ******************************************************************************/
 
 /*!
 * @brief This function parse string into argv[].
 */
static int make_argv(char *s, int argvsz, char *argv[])
{
    uint8_t argc = 0;
    /* split into argv */
    while (argc < argvsz - 1)
    {
        /* skip any white space */
        while (isblank(*s))
				{
					++s;
				}
				if (*s == '\0')	/* end of s, no more args	*/
				{
				    break;
				}    
				argv[argc++] = s;	/* begin of argument string	*/
				/* find end of string */
				while (*s && !isblank(*s))
				++s;
				if (*s == '\0')		/* end of s, no more args	*/
				{
				    break;
				}				
				*s++ = '\0';		/* terminate current arg	 */
    }
    argv[argc] = NULL;
    return argc;
}

 /*!
 * @brief find the available matchs and return possiable match number.
 */
static int complete_cmdv(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
    cmd_tbl_t *cmdtp = NULL;
		cmd_tbl_t **cmdtpt = SHELL_get_cmd_tbl();
	  uint8_t i;
    uint8_t clen;
    uint8_t n_found = 0;
    /* sanity */
    if (maxv < 2)
    {
        return -2;
		}
    cmdv[0] = NULL;
    if (argc == 0) 
    {
			  /* no argment, only typed TAB   */
        /* output full list of commands */
				i = 0;
				while((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
				{
            if (n_found >= maxv - 2)
            {
                cmdv[n_found] = "...";
                break;
            }
            cmdv[n_found] = cmdtpt[n_found]->name; 
            i++;
						n_found++;
				}
        return n_found;
    }
    /* more than one arg or one but the start of the next */
    if (argc > 1 || (last_char == '\0' || isblank(last_char)))
    {
        cmdtp = SHELL_find_command(argv[0]);
        if (cmdtp == NULL || cmdtp->complete == NULL) 
        {
            cmdv[0] = NULL;
            return 0;
        }
        return (*cmdtp->complete)(argc, argv, last_char, maxv, cmdv);
    }
    /*
    * only one argument
    * Some commands allow length modifiers (like "cp.b");
    * compare command name only until first dot.
    */
		i = 0;
		n_found = 0;
		while((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
		{
        clen = strlen(cmdtpt[i]->name);
        if (clen < strlen(argv[0]))
        {
					  i++;
            continue;
        }
        if (memcmp(argv[0], cmdtpt[i]->name, strlen(argv[0])) != 0)
        {
					  i++;
            continue;
        }
        /* too many! */
        if (n_found >= maxv - 2)
        {
            cmdv[n_found++] = "...";
            break;
        }
        cmdv[n_found++] = cmdtpt[i]->name;
				i++;
		}
    cmdv[n_found] = NULL;
    return n_found;
}

 /*!
 * @brief print possible matchs
 */
static void print_argv(const char *banner, const char *leader, const char *sep, int linemax, char * const argv[])
{
    uint8_t ll = leader != NULL ? strlen(leader) : 0;
    uint8_t sl = sep != NULL ? strlen(sep) : 0;
    uint8_t len, i;
    if (banner)
    {
        SHELL_printf("\r\n%s", banner);
    }
    i = linemax;	/* force leader and newline */
    while (*argv != NULL)
    {
        len = strlen(*argv) + sl;
        if (i + len >= linemax)
        {
            SHELL_printf("\r\n");
            if (leader)
            {
                SHELL_printf(leader);
						}
            i = ll - sl;
        }
        else if (sep)
				{
            SHELL_printf(sep);
				}
        SHELL_printf(*argv++);
        i += len;
    }
    SHELL_printf("\r\n");
}

 /*!
 * @brief find the number of common characters of matchs.
 */
static int find_common_prefix(char * const argv[])
{
    uint8_t i, len;
    char *anchor, *s, *t;
    if (*argv == NULL)
		{
        return 0;
		}
	/* begin with max */
    anchor = *argv++;
    len = strlen(anchor);
    while ((t = *argv++) != NULL)
    {
        s = anchor;
        for (i = 0; i < len; i++, t++, s++)
        {
            if (*t != *s)
            {
                break;
            }       
        }
    len = s - anchor;
    }
    return len;
}


 /*!
 * @brief auto complete interface function.
 */
int cmd_auto_complete(const char *const prompt, char *buf, uint8_t *np, uint8_t *colp)
{
    uint8_t n = *np, col = *colp;
    char *argv[20 + 1];		/* NULL terminated	*/
    char *cmdv[20];
    char *s, *t;
    const char *sep;
    uint8_t i, j, k, len, seplen, argc;
    uint8_t cnt;
    char last_char;
    cnt = strlen(buf);
    if (cnt >= 1)
		{
			last_char = buf[cnt - 1];
		}
    else
		{
			last_char = '\0';
		}
    /* copy to secondary buffer which will be affected */
    strcpy(tmp_buf, buf);

    /* separate into argv */
    argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);
    /* do the completion and return the possible completions */
    i = complete_cmdv(argc, argv, last_char, sizeof(cmdv)/sizeof(cmdv[0]), cmdv);

    /* no match; bell and out */
    if (i == 0)
    {
        if (argc > 1)	/* allow tab for non command */
				{
            return 0;
				}
        SHELL_beep();
        return 1;
    }
    s = NULL;
    len = 0;
    sep = NULL;
    seplen = 0;
    if (i == 1)
    { /* one match; perfect */
        k = strlen(argv[argc - 1]);
        s = cmdv[0] + k;
        len = strlen(s);
        sep = " ";
        seplen = 1;
    }
		else if (i > 1 && (j = find_common_prefix(cmdv)) != 0)
    {	/* more */
        k = strlen(argv[argc - 1]);
        j -= k;
        if (j > 0)
        {
            s = cmdv[0] + k;
            len = j;
        }
    }
    if (s != NULL)
    {
        k = len + seplen;
        /* make sure it fits */
        if (n + k >= SHELL_CB_SIZE - 2) 
        {
            SHELL_beep();
            return 1;
        }
        t = buf + cnt;
        for (i = 0; i < len; i++)
        *t++ = *s++;
        if (sep != NULL)
        for (i = 0; i < seplen; i++)
        *t++ = sep[i];
        *t = '\0';
        n += k;
        col += k;
        SHELL_printf(t - k);
        if (sep == NULL)
        SHELL_beep();
        *np = n;
        *colp = col;
    }
    else
    {
        print_argv(NULL , "  ", " ", 78, cmdv);
        SHELL_printf(prompt);
        SHELL_printf(buf);
    }
    return 1;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

