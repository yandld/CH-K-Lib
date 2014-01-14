#include "shell.h"
#include "systick.h"
#include "common.h"

static int DoMPU(int argc, char *const argv[])
{
    uint8_t str_len;
    shell_printf("Enter dealy Test\r\n");
    DelayInit();
    shell_printf("DelayInit complete\r\n");
    while(1)
    {
        shell_printf("lifetimetick:%dms\r\n", 123);
        DelayUs(1000*10);
        DelayMs(400);
    }
    return 0;
}

const cmd_tbl_t CommandFun_DELAY = 
{
    .name = "DELAY",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoMPU,
    .usage = "DELAY",
    .complete = NULL,
    .help = "DELAY\r\n"

};


