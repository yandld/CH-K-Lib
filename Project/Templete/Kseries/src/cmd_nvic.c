#include "common.h"
#include "shell/shell.h"


int CMD_NVIC(int argc, char * const * argv)
{
    SystemSoftReset();
    return 0;
}

const cmd_tbl_t CommandFun_NVIC = 
{
    .name = "NVIC",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_NVIC,
    .usage = "NVIC",
    .complete = NULL,
    .help = "\r\n"
};
