#include "shell.h"
#include "i2c.h"


int DoI2C(int argc, char *const argv[])
{
    uint8_t i;
    I2C_QuickInit(I2C1_SCL_PC10_SDA_PC11, 240*1000);
   // I2C_WriteSingleRegister(I2C1, 0x44, 3, 44);
}


const cmd_tbl_t CommandFun_I2C = 
{
    .name = "I2C",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoI2C,
    .usage = "I2C",
    .complete = NULL,
    .help = "\r\n"
    "I2C clock - print CPU clock\r\n"
    "I2C memory   - print CPU memory info"
};



