#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
    uint8_t (*getc)(void);
    void    (*putc)(uint8_t ch);
}SHELL_io_install_t;

#define CONFIG_USE_STDOUT
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_LONGHELP

#define SHELL_CB_SIZE	      		 64
#define SHELL_MAX_ARGS		        5
#define SHELL_MAX_FUNCTION_NUM  (64)
#define HIST_MAX                 20
#define HIST_SIZE		             SHELL_CB_SIZE




#ifdef CONFIG_USE_STDOUT
#define SHELL_printf   printf
#endif

typedef struct  
{
    char		*name;		/* Command Name			*/
    uint8_t		maxargs;	/* maximum number of arguments	*/
    uint8_t		repeatable;	/* autorepeat allowed?		*/
    int		(*cmd)(int, char * const []);  /* Implementation function	*/
    char		*usage;		/* Usage message	(short)	*/
#ifdef	CONFIG_SYS_LONGHELP
    char		*help;		/* Help  message	(long)	*/
#endif
#ifdef CONFIG_AUTO_COMPLETE
    int		(*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]); 	/* do auto completion on the arguments */
#endif
}cmd_tbl_t;

//!< API funcion
uint8_t SHELL_register_function(const cmd_tbl_t* pAddress);
uint8_t SHELL_unregister_function(char* name);
int SHELL_printf(const char *format,...);
void SHELL_beep(void);
cmd_tbl_t *SHELL_find_command (const char *cmd);
uint8_t SHELL_io_install(SHELL_io_install_t* IOInstallStruct);
cmd_tbl_t **SHELL_get_cmd_tbl(void);

#endif
