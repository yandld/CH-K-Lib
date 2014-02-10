






#include "flexbus.h"
#include "shell/shell.h"



int CMD_FLEXBUS(int argc, char * const * argv)
{
    uint8_t instance;
    shell_printf("FLEXBUS Test CMD\r\n");
    FLEXBUS_InitTypeDef FLEXBUS_InitStruct;
    FLEXBUS_InitStruct.addressSpaceInByte = 512*1024;
    FLEXBUS_Init(&FLEXBUS_InitStruct);
    return 0;
}



const cmd_tbl_t CommandFun_FLEXBUS = 
{
    .name = "FLEXBUS",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_FLEXBUS,
    .usage = "FLEXBUS TEST",
    .complete = NULL,
    .help = "\r\n"
};
