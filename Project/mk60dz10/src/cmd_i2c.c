#include "shell.h"
#include "i2c.h"

static int _do_i2c_scan(int argc, char *const argv[])
{
    
    
}


int DoI2C(int argc, char *const argv[])
{
    uint8_t i;
    static uint8_t init = 0;
    if(!init)
    {
        I2C_QuickInit(I2C1_SCL_PC10_SDA_PC11, 240*1000);
        init = 1;
    }
    if(!strcmp("scan", argv[2]))
    {
        _do_i2c_scan(argc, argv);
        
    }
    I2C_WriteSingleRegister(I2C1, 0x44, 3, 44);
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



