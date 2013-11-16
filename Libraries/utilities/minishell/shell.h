#ifndef __SHELL_H__
#define __SHELL_H__

#include "uart.h"



typedef struct
{
    char *name;                 //!< Command name
    int  maxargs;               //!< max args
    int (*cmd)(int, char *[]);  //!< Implementation function
    char *usage;                //!< Usage message (short)
}SHELL_CommandTableTypeDef;


int SHELL_printf(const char *format,...);
void SHELL_CmdHandleLoop(char *name);



#endif


