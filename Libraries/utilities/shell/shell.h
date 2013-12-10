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
#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "shell_config.h"

typedef struct
{
    uint8_t (*getc)(void);
    void    (*putc)(uint8_t ch);
}SHELL_io_install_t;

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
    char		*help;		/* Help  message	(long)	*/
    int		(*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]); 	/* do auto completion on the arguments */
}cmd_tbl_t;

typedef enum
{
	CMD_RET_SUCCESS,	/* 0 = Success */
	CMD_RET_FAILURE,	/* 1 = Failure */
	CMD_RET_USAGE = -1,	/* Failure, please report 'usage' error */
}command_ret_t;

//!< API funcions
uint8_t SHELL_register_function(const cmd_tbl_t* pAddress);
void SHELL_register_function_array(const cmd_tbl_t* pAddress, uint8_t num);
uint8_t SHELL_unregister_function(char* name);
int SHELL_printf(const char *format,...);
void SHELL_beep(void);
cmd_tbl_t *SHELL_find_command (const char *cmd);
uint8_t SHELL_io_install(SHELL_io_install_t* IOInstallStruct);
cmd_tbl_t **SHELL_get_cmd_tbl(void);
char **SHELL_get_hist_data_list(uint8_t* num);
void SHELL_main_loop(char* prompt);

#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
 
 
