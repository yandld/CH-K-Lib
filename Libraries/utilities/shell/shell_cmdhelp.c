#include "shell.h"

/*******************************************************************************
 * Defination
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
 
 /*******************************************************************************
 * Code
 ******************************************************************************/
 
 
 /*!
 * @brief help command.
 */
int DoHelp(int argc, char *const argv[])
{
    uint8_t i;
    cmd_tbl_t **cmdtpt = SHELL_get_cmd_tbl();
    if(argc == 1)
    {
			  i = 0;
        while((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
				{
					SHELL_printf("%-*s(%d)- %s\r\n", 8, cmdtpt[i]->name, i, cmdtpt[i]->usage);
					i++;
				}
    }
		if(argc == 2)
		{
			  i = 0;
        while((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
				{
             if(!strcmp(argv[1], cmdtpt[i]->name))
						 {
							 if(cmdtpt[i]->help != NULL)
							 {
								 SHELL_printf("%-*s- %s\r\n", 8, cmdtpt[i]->name, cmdtpt[i]->help);
								 return 0;
							 }
							 else
							 {
								 SHELL_printf ("- No additional help available.\r\n");
								 return 0;
							 }
						 }
						 i++;
				}
				SHELL_printf ("- No command available.\r\n");
		}
		return 0;
}

 /*!
 * @brief help command auto complete function.
 */
int DoHelpComplete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
    uint8_t str_len;
    uint8_t found = 0;
    uint8_t i;
    str_len = strlen(argv[argc-1]);
    cmd_tbl_t **cmdtpt = SHELL_get_cmd_tbl();
    switch(argc)
    {
        case 2:
					  i = 0;
            while((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
						{
                if(!strncmp(argv[argc-1], cmdtpt[i]->name, str_len))
                {
                    cmdv[found] = cmdtpt[i]->name;
                    found++;
                }  
                i++;
						}
            break;
        default:
            break;
    }
    return found;
}


const cmd_tbl_t CommandFun_Help = 
{
    .name = "help",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = DoHelp,
    .usage = "print command description/usage",
    .complete = DoHelpComplete,
    .help = "\r\n"
            "	- print brief description of all commands\r\n"
            "help <command>\r\n"
            "	- print detailed usage of 'command'\r\n"
};

/*******************************************************************************
 * EOF
 ******************************************************************************/

