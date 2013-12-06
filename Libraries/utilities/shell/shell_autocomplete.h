#ifndef __SHELL_AUTO_COMPLETE_H__
#define __SHELL_AUTO_COMPLETE_H__

#include "shell.h"


#define CONFIG_SYS_MAXARGS 20
#define  CONFIG_SYS_PROMPT  "MS>>"
#define  CONFIG_SYS_CBSIZE  128

cmd_tbl_t *find_cmd (const char *cmd);

#endif
