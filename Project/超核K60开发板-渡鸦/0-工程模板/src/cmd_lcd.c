
#include "shell.h"
#include "ili9320.h"

int CMD_LCD(int argc, char * const * argv)
{
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED\r\n");
    return 0;
#else
    shell_printf("PLEASE INIT FLEXBUS FIRST\r\n");
    ili9320_Init();
    shell_printf("LCD ID:0x%X\r\n", ILI9320_GetDeivceID());
    
    return 0;
#endif
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
