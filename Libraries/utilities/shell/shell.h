/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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
#ifndef __shell_H__
#define __shell_H__

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "shell_config.h"

typedef struct
{
    uint8_t (*sh_getc)(void);
    void    (*sh_putc)(uint8_t ch);
}shell_io_install_t;

#ifdef SHELL_CONFIG_USE_STDIO
#define shell_printf(fmt,args...)	printf (fmt ,##args)
#endif

typedef struct  
{
    char		*name;		/* Command Name			*/
    uint8_t		maxargs;	/* maximum number of arguments	*/
    uint8_t		repeatable;	/* autorepeat allowed?		*/
    int		(*cmd)(int argc, char * const argv[]);  /* Implementation function	*/
    char		*usage;		/* Usage message	(short)	*/
    char		*help;		/* Help  message	(long)	*/
    int		(*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]); 	/* do auto completion on the arguments */
}cmd_tbl_t;

typedef enum
{
    CMD_RET_SUCCESS = 0,    /* 0 = Success */
    CMD_RET_FAILURE = 1,    /* 1 = Failure */
    CMD_RET_USAGE = -1,     /* Failure, please report 'usage' error */
}command_ret_t;

//!< API funcions
uint8_t shell_register_function(const cmd_tbl_t * pAddress);
void shell_register_function_array(const cmd_tbl_t * pAddress, uint8_t num);
uint8_t shell_unregister_function(char * name);
int shell_printf(const char * format,...);
void shell_beep(void);
const cmd_tbl_t *shell_find_command (const char * cmd);
uint8_t shell_io_install(shell_io_install_t * IOInstallStruct);
const cmd_tbl_t ** shell_get_cmd_tbl(void);
char ** shell_get_hist_data_list(uint8_t * num, uint8_t * cur_index);
void shell_main_loop(char * prompt);

#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
 
 
