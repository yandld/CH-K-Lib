#include "shell.h"
#include "i2c.h"
#include "board.h"
#include "at24cxx.h"
#include "i2c_abstraction.h"


static const uint32_t I2C_TestSpeedTable[] = {47000, 76000, 96000, 376000};
static uint32_t gI2C_Instance;

static int _do_i2c_scan(int argc, char *const argv[])
{
    uint8_t i,j;
    
    for(j = 0; j < ARRAY_SIZE(I2C_TestSpeedTable); j++)
    {
        I2C_ABS_Init(kI2C_ABS_SpeedStandard);
        shell_printf("Scanning I2C%d bus at:%dHz\r\n",gI2C_Instance,  I2C_TestSpeedTable[j]);
        for(i=0;i<127;i++)
        {
            if(I2C_ABS_Probe(i) == kI2C_ABS_StatusOK)
            {
                shell_printf("address:0x%X(0x%X) found!\r\n", i, i<<1);
            }
        }
    }
}

void I2C_ISR(void)
{
    shell_printf("enter i2c it\r\n");
}

static int _do_i2c_it(int argc, char *const argv[])
{
    gI2C_Instance = I2C_QuickInit(BOARD_I2C_MAP, 47000);
    shell_printf("Scanning I2C%d bus at:%dHz\r\n",gI2C_Instance, 47000);
    uint8_t i2c_7bit_address = strtoul(argv[2], 0, 0);
    shell_printf("i2c it test Address:0x%x(%d)\r\n", i2c_7bit_address, i2c_7bit_address);
    I2C_CallbackInstall(gI2C_Instance, I2C_ISR);
    I2C_ITDMAConfig(gI2C_Instance, kI2C_IT_BTC);
    I2C_GenerateSTART(gI2C_Instance);
    I2C_Send7bitAddress(gI2C_Instance, i2c_7bit_address, kI2C_Write);
    shell_printf("waitting for IT...\r\n");
    DelayMs(200);
    I2C_GenerateSTOP(gI2C_Instance);
    I2C_ITDMAConfig(gI2C_Instance, kI2C_IT_Disable);
}



static int _do_i2c_at24cxx(int argc, char *const argv[])
{
    uint32_t ret;
    uint8_t buffer[256];
    uint32_t i = 0;
    AT24CXX_Init(kAT24C02);
    ret = AT24CXX_SelfTest();
    if(ret)
    {
        shell_printf("AT24CXX FAILED:%d\r\n", ret);
    }
    else
    {
        shell_printf("AT24CXX OK\r\n", ret);   
    }
}

int DoI2C(int argc, char *const argv[])
{
    uint8_t i;
    static uint8_t init = 0;
    if(argc == 1)
    {
        return CMD_RET_USAGE;
    }
    if(!strcmp("SCAN", argv[1]))
    {
        _do_i2c_scan(argc, argv);
        return 0;
    }
    if(!strcmp("IT", argv[1]) && (argc == 3))
    {
        _do_i2c_it(argc, argv);
        return 0;
    }
    if(!strcmp("AT24CXX", argv[1]))
    {
        _do_i2c_at24cxx(argc, argv);
        return 0;
    }
    else
    {
        return CMD_RET_USAGE;
    }
    return 0;
}


const cmd_tbl_t CommandFun_I2C = 
{
    .name = "I2C",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoI2C,
    .usage = "I2C <CMD>",
    .complete = NULL,
    .help = "\r\n"
    "I2C <CMD>\r\n"
    "eg: I2C SCAN\r\n"
    "eg: I2C IT 55\r\n"
};

