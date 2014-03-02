#include "common.h"
#include "shell.h"


int CMD_RESET(int argc, char * const * argv)
{
    SystemSoftReset();
    return 0;
}

const cmd_tbl_t CommandFun_RESET = 
{
    .name = "RESET",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_RESET,
    .usage = "RESET",
    .complete = NULL,
    .help = "\r\n"
};
