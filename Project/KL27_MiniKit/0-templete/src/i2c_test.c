#include "at24cxx.h"
#include "i2c.h"

int cmd_i2c(int argc, char * const argv[])
{
    int ret;
    I2C_QuickInit(I2C1_SCL_PC01_SDA_PC02, 100*1000);
    at24cxx_init(1);
    ret = at24cxx_self_test();
    return ret;
}

