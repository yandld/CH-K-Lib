#include "shell.h"
#include "i2c.h"
#include "board.h"
#include "24cxx.h"

static uint8_t gI2C_Instance = 0;

static const uint32_t I2C_TestSpeedTable[] = {47000, 76000, 96000, 376000};

static int _do_i2c_scan(int argc, char *const argv[])
{
    uint8_t i,j;
    
    for(j = 0; j < ARRAY_SIZE(I2C_TestSpeedTable); j++)
    {
        gI2C_Instance = I2C_QuickInit(BOARD_I2C_MAP, I2C_TestSpeedTable[j]);
        shell_printf("Scanning I2C%d bus at:%dHz\r\n",gI2C_Instance,  I2C_TestSpeedTable[j]);
        for(i=0;i<127;i++)
        {
           
            I2C_GenerateSTART(gI2C_Instance);
            I2C_Send7bitAddress(gI2C_Instance, i, kI2C_Write);
            if(I2C_WaitAck(gI2C_Instance))
            {
                I2C_GenerateSTOP(gI2C_Instance);
                while(!I2C_IsBusy(gI2C_Instance));
            }
            else
            {
                I2C_GenerateSTOP(gI2C_Instance);
                while(!I2C_IsBusy(gI2C_Instance)); 
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
    printf("SIZE:%dByte\r\n", AT24CXX_GetTotalSize(kAT24C02));
    memset(buffer, 'G', sizeof(buffer));
    AT24CXX_WriteByte(0, buffer, sizeof(buffer));
    printf("ret:%d\r\n", ret);
    memset(buffer, '0', sizeof(buffer));
    AT24CXX_ReadByte(0, buffer, sizeof(buffer));
    //I2C_ReadSingleRegister(HW_I2C0,0x50,0,buffer);
    printf("ret:%d\r\n", ret);
    for(i=0;i< sizeof(buffer);i++)
    {
        printf("[%d]:%c\r\n", i, buffer[i]);
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



