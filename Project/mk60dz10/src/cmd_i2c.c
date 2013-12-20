#include "shell.h"
#include "i2c.h"

static int _do_i2c_scan(int argc, char *const argv[])
{
    uint8_t i;
    shell_printf("scanning I2c bus...\r\n");
    for(i=0;i<0x7F;i++)
    {
        I2C_GenerateSTART(1);
        I2C_Send7bitAddress(1, i, kI2C_Write);
        if(I2C_WaitAck(1))
        {
            I2C_GenerateSTOP(1);
            while(!I2C_IsBusy(1));
        }
        else
        {
            I2C_GenerateSTOP(1);
            while(!I2C_IsBusy(1)); 
            shell_printf("address:0x%x found!\r\n", i);
        }
    }
}

void I2C_ISR(uint8_t data)
{
    
    shell_printf("enter\r\n");
}

static int _do_i2c_it(int argc, char *const argv[])
{
    shell_printf("i2c it test...\r\n");
    I2C_ITDMAConfig(1, kI2C_IT_BTC, ENABLE);
        I2C_GenerateSTART(1);
        I2C_Send7bitAddress(1, 0x33, kI2C_Write);
        if(I2C_WaitAck(1))
        {
            I2C_GenerateSTOP(1);
            while(!I2C_IsBusy(1));
        }
        else
        {
            I2C_GenerateSTOP(1);
            while(!I2C_IsBusy(1));    
        }
    I2C_CallbackInstall(1, I2C_ISR);
}

int DoI2C(int argc, char *const argv[])
{
    uint8_t i;
    static uint8_t init = 0;
    if(!init)
    {
        I2C_QuickInit(I2C1_SCL_PC10_SDA_PC11, 96*1000);
        init = 1;
    }
    if(!strcmp("scan", argv[1]))
    {
        _do_i2c_scan(argc, argv);
        return 0;
    }
    if(!strcmp("it", argv[1]))
    {
        _do_i2c_it(argc, argv);
        return 0;
    }
    return 0;
   // I2C_WriteSingleRegister(1, 0x44, 3, 44);
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



