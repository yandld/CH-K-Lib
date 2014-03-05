
#include "shell.h"
#include "sram.h"

int CMD_SRAM(int argc, char * const * argv)
{
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED\r\n");
    return 0;
#else
    uint32_t err_cnt;
    SRAM_Init();
    err_cnt = SRAM_SelfTest();
    if(err_cnt)
    {
        shell_printf("SRAM ERROR CNT:%d\r\n", err_cnt);
    }
    else
    {
        shell_printf("SRAM TEST PASSED\r\n");
    }
    return 0;
#endif
}

const cmd_tbl_t CommandFun_SRAM = 
{
    .name = "SRAM",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_SRAM,
    .usage = "SRAM",
    .complete = NULL,
    .help = "\r\n"
};
