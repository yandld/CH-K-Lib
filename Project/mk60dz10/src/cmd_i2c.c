#include "shell.h"
#include "i2c.h"



const cmd_tbl_t CommandFun_I2C = 
{
    .name = "I2C",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = NULL,
    .usage = "I2C",
    .complete = NULL,
    .help = "\r\n"
		        "I2C clock - print CPU clock\r\n"
						"I2C memory   - print CPU memory info"
};



