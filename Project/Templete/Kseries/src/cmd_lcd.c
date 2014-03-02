
#include "shell.h"
#include "ili9320.h"

int CMD_LCD(int argc, char * const * argv)
{
    uint32_t err_cnt;
    ili9320_Init();
    return 0;
}

const cmd_tbl_t CommandFun_LCD = 
{
    .name = "LCD",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_LCD,
    .usage = "LCD",
    .complete = NULL,
    .help = "\r\n"
};
