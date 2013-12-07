#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>


typedef struct
{
    uint8_t (*getc)(void);
    void    (*putc)(uint8_t ch);
}SHELL_IOInstall_Type;

typedef enum
{
    kShell_Success = 0,
		kShell_Fail,
}SHELL_Status_Type;


#define SHELL_CB_SIZE			64
#define SHELL_MAX_ARGS		5
#define SHELL_MAX_FUNCTION_NUM  (64)

#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_LONGHELP


struct cmd_tbl_s 
{
	char		*name;		/* Command Name			*/
	int		maxargs;	/* maximum number of arguments	*/
	int		repeatable;	/* autorepeat allowed?		*/
	int		(*cmd)(struct cmd_tbl_s *, int, int, char * const []);  /* Implementation function	*/
	char		*usage;		/* Usage message	(short)	*/
#ifdef	CONFIG_SYS_LONGHELP
	char		*help;		/* Help  message	(long)	*/
#endif
#ifdef CONFIG_AUTO_COMPLETE
	int		(*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]); 	/* do auto completion on the arguments */
#endif
};

typedef struct cmd_tbl_s	cmd_tbl_t;



int readline (const char *const prompt);

#endif