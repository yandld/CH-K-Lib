#include "shell.h"
#include "gpio.h"



int DoGPIO(int argc, char * argv[])
{
    
    
}

const cmd_tbl_t CommandFun_GPIO = 
{
    .name = "GPIO",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoGPIO,
    .usage = "GPIO",
    .complete = NULL,
    .help = "\r\n"
};

