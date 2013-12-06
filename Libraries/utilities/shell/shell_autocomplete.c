#include "shell_autocomplete.h"
#include "shell.h"
#include "common.h"
#include "string.h"





int CommandFun1(struct cmd_tbl_s *cmd_tp, int flag, int argc, char *const argv[])
{
	UART_printf("I am the Test CommandFun1\r\n");
	UART_printf("flag:%d\r\n", flag);
	while(argc--)
	{
		UART_printf("ARGV[%d]:%s\r\n", argc, argv[argc]);
	}
}

int CommandFun1Complete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
    uint8_t str_len;
    uint8_t found = 0;

    str_len = strlen(argv[argc-1]);
    if(!strncmp(argv[argc-1], "help", str_len))
    {
        cmdv[found] = "help";
        cmdv[found+1] = NULL;
        found++;
    }
    if(!strncmp(argv[argc-1], "hexx", str_len))
    {
        cmdv[found] = "hexx";
        cmdv[found+1] = NULL;
        found++;
    }
    return found;
}


const cmd_tbl_t MyCommand1 = 
{
	.name = "test",
	.maxargs = 5,
	.repeatable = 1,
	.cmd = CommandFun1,
	.usage = "Help on my function",
	.complete = CommandFun1Complete,
};

const cmd_tbl_t MyCommand2 = 
{
	.name = "ttt",
	.maxargs = 5,
	.repeatable = 1,
	.cmd = CommandFun1,
	.usage = "Help on my function2",
	.complete = CommandFun1Complete,
};

cmd_tbl_t gCMD_Table[5] = {NULL,NULL,NULL,NULL,NULL};



/***************************************************************************
 * find command table entry for a command
 */
cmd_tbl_t *find_cmd_tbl (const char *cmd, cmd_tbl_t *table, int table_len)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdtp_temp = table;	/*Init value */
	const char *p;
	int len;
	int n_found = 0;

	if (!cmd)
		return NULL;
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);

	for (cmdtp = table;
	     cmdtp != table + table_len;
	     cmdtp++) {
		if (strncmp (cmd, cmdtp->name, len) == 0) {
			if (len == strlen (cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return cmdtp_temp;
	}

	return NULL;	/* not found or ambiguous command */
}




cmd_tbl_t *find_cmd (const char *cmd)
{
	//cmd_tbl_t *start = ll_entry_start(cmd_tbl_t, cmd);
	//const int len = ll_entry_count(cmd_tbl_t, cmd);
	gCMD_Table[0] = MyCommand1;
	gCMD_Table[1] = MyCommand2;
	return find_cmd_tbl(cmd, gCMD_Table, 2);
}

static int make_argv(char *s, int argvsz, char *argv[])
{
	int argc = 0;

	/* split into argv */
	while (argc < argvsz - 1) {

		/* skip any white space */
		while (isblank(*s))
			++s;

		if (*s == '\0')	/* end of s, no more args	*/
			break;

		argv[argc++] = s;	/* begin of argument string	*/

		/* find end of string */
		while (*s && !isblank(*s))
			++s;

		if (*s == '\0')		/* end of s, no more args	*/
			break;

		*s++ = '\0';		/* terminate current arg	 */
	}
	argv[argc] = NULL;

	return argc;
}




/*************************************************************************************/

static int complete_cmdv(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{

  	gCMD_Table[0] = MyCommand1;
		gCMD_Table[1] = MyCommand2;
	cmd_tbl_t *cmdtp = gCMD_Table;
	const int count = 2;
	
	const cmd_tbl_t *cmdend = cmdtp + count;
	const char *p;
	int len, clen;
	int n_found = 0;
	const char *cmd;

	/* sanity? */
	if (maxv < 2)
		return -2;

	cmdv[0] = NULL;

	if (argc == 0) {
		/* output full list of commands */
		for (; cmdtp != cmdend; cmdtp++) {
			if (n_found >= maxv - 2) {
				cmdv[n_found] = "...";
				break;
			}
			cmdv[n_found] = cmdtp->name;
		}
		cmdv[n_found] = NULL;
		return n_found;
	}

	/* more than one arg or one but the start of the next */
	if (argc > 1 || (last_char == '\0' || isblank(last_char))) {
		cmdtp = find_cmd(argv[0]);
		if (cmdtp == NULL || cmdtp->complete == NULL) {
			cmdv[0] = NULL;
			return 0;
		}
		return (*cmdtp->complete)(argc, argv, last_char, maxv, cmdv);
	}

	cmd = argv[0];
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	p = strchr(cmd, '.');
	if (p == NULL)
		len = strlen(cmd);
	else
		len = p - cmd;

	/* return the partial matches */
	for (; cmdtp != cmdend; cmdtp++) {

		clen = strlen(cmdtp->name);
		if (clen < len)
			continue;

		if (memcmp(cmd, cmdtp->name, len) != 0)
			continue;

		/* too many! */
		if (n_found >= maxv - 2) {
			cmdv[n_found++] = "...";
			break;
		}

		cmdv[n_found++] = cmdtp->name;
	}

	cmdv[n_found] = NULL;
	return n_found;
}








static void print_argv(const char *banner, const char *leader, const char *sep, int linemax, char * const argv[])
{
	int ll = leader != NULL ? strlen(leader) : 0;
	int sl = sep != NULL ? strlen(sep) : 0;
	int len, i;

	if (banner) {
		mputs("\r\n");
		mputs(banner);
	}

	i = linemax;	/* force leader and newline */
	while (*argv != NULL) {
		len = strlen(*argv) + sl;
		if (i + len >= linemax) {
			mputs("\r\n");
			if (leader)
				mputs(leader);
			i = ll - sl;
		} else if (sep)
			mputs(sep);
		mputs(*argv++);
		i += len;
	}
	printf("\r\n");
}



static int find_common_prefix(char * const argv[])
{
	int i, len;
	char *anchor, *s, *t;

	if (*argv == NULL)
		return 0;

	/* begin with max */
	anchor = *argv++;
	len = strlen(anchor);
	while ((t = *argv++) != NULL) {
		s = anchor;
		for (i = 0; i < len; i++, t++, s++) {
			if (*t != *s)
				break;
		}
		len = s - anchor;
	}
	return len;
}



static char tmp_buf[CONFIG_SYS_CBSIZE];	/* copy of console I/O buffer	*/


int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp)
{
	int n = *np, col = *colp;
	char *argv[CONFIG_SYS_MAXARGS + 1];		/* NULL terminated	*/
	char *cmdv[20];
	char *s, *t;
	const char *sep;
	int i, j, k, len, seplen, argc;
	int cnt;
	char last_char;

	if (strcmp(prompt, CONFIG_SYS_PROMPT) != 0)
		return 0;	/* not in normal console */

	cnt = strlen(buf);
	if (cnt >= 1)
		last_char = buf[cnt - 1];
	else
		last_char = '\0';

	/* copy to secondary buffer which will be affected */
	strcpy(tmp_buf, buf);

	/* separate into argv */
	argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);

	/* do the completion and return the possible completions */
	i = complete_cmdv(argc, argv, last_char, sizeof(cmdv)/sizeof(cmdv[0]), cmdv);

	/* no match; bell and out */
	if (i == 0) {
		if (argc > 1)	/* allow tab for non command */
			return 0;
		putc('\a');
		return 1;
	}

	s = NULL;
	len = 0;
	sep = NULL;
	seplen = 0;
	if (i == 1) { /* one match; perfect */
		k = strlen(argv[argc - 1]);
		s = cmdv[0] + k;
		len = strlen(s);
		sep = " ";
		seplen = 1;
	} else if (i > 1 && (j = find_common_prefix(cmdv)) != 0) {	/* more */
		k = strlen(argv[argc - 1]);
		j -= k;
		if (j > 0) {
			s = cmdv[0] + k;
			len = j;
		}
	}

	if (s != NULL) {
		k = len + seplen;
		/* make sure it fits */
		if (n + k >= CONFIG_SYS_CBSIZE - 2) {
			putc('\a');
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
		mputs(t - k);
		if (sep == NULL)
			putc('\a');
		*np = n;
		*colp = col;
	} else {
		print_argv(NULL, "  ", " ", 78, cmdv);

		mputs(prompt);
		mputs(buf);
	}
	return 1;
}


