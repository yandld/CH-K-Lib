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
static int DoHelp(int argc, char * const argv[])
{
    uint8_t i;
    const cmd_tbl_t ** cmdtpt = shell_get_cmd_tbl();
    if (argc == 1)
    {
        i = 0;
        while ((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
        {
            shell_printf("%-*s(%d)- %s\r\n", 8, cmdtpt[i]->name, i, cmdtpt[i]->usage);
            i++;
        }
    }
    if (argc == 2)
    {
        i = 0;
        while ((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
        {
            if (!strcmp(argv[1], cmdtpt[i]->name))
            {
                if (cmdtpt[i]->help != NULL)
                {
                    shell_printf("%-*s- %s\r\n", 8, cmdtpt[i]->name, cmdtpt[i]->help);
                    return CMD_RET_SUCCESS;
                }
                else
                {
                    shell_printf ("- No additional help available.\r\n");
                    return CMD_RET_SUCCESS;
                }
            }
            i++;
        }
        shell_printf ("- No command available.\r\n");
    }
    return CMD_RET_SUCCESS;
}

 /*!
 * @brief help command auto complete function.
 */
static int DoHelpComplete(int argc, char * const argv[], char last_char, int maxv, char * cmdv[])
{
    uint8_t str_len;
    uint8_t found = 0;
    uint8_t i;
    str_len = strlen(argv[argc-1]);
    const cmd_tbl_t **cmdtpt = shell_get_cmd_tbl();
    switch(argc)
    {
        case 2:
            i = 0;
            while ((cmdtpt[i] != NULL) && (i < SHELL_MAX_FUNCTION_NUM))
            {
                if (!strncmp(argv[argc-1], cmdtpt[i]->name, str_len))
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

