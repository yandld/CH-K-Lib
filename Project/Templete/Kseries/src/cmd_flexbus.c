
#include "flexbus.h"
#include "shell.h"
#include "ili9320.h"


int CMD_FLEXBUS(int argc, char * const * argv)
{
    uint8_t instance;
    shell_printf("FLEXBUS Test CMD\r\n");
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED FEATURE\r\n");
    
#else
    
   // ili9320_Init();
  //  SRAM_Init();
#endif

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
