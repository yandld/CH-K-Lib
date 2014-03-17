#include "shell.h"
#include "i2c.h"
#include "board.h"
#include "at24cxx.h"
#include "i2c_abstraction.h"


static uint32_t gI2C_Instance;

static void _do_i2c_scan(int argc, char *const argv[])
{
    uint8_t i,j;
    i2c_bus bus;
    if(i2c_bus_init(&bus, BOARD_I2C_INSTANCE, 10*1000))
    {
        printf("i2c init failed\r\n");
        return;
    }
    shell_printf("i2c scanning bus at speed:%d\r\n", bus.baudrate);
    for(i = 0;i < 127; i++)
    {
        if(bus.probe(&bus, i) == ki2c_status_ok)
        {
            shell_printf("address:0x%X(0x%X) found!\r\n", i, i<<1);
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
    uint32_t i;
    uint8_t buffer[10];
    uint32_t size;
    i2c_bus bus = {0};
    at24cxx_device at24cxx = {0};
    if(i2c_bus_init(&bus, BOARD_I2C_INSTANCE, 40*1000))
    {
        shell_printf("i2c bus init failed\r\n");
    }
    at24cxx.bus = &bus;
    if(at24cxx_init(&at24cxx))
    {
        shell_printf("init at24cxx failed\r\n");
        return 1;
    }
    if(at24cxx.probe(&at24cxx, kAT24C02))
    {
        shell_printf("no device found\r\n");
        return 1;
    }
    at24cxx.get_size(&at24cxx, &size);
    shell_printf("at24cxx size:%d btye\r\n", size);
    if(at24cxx.self_test(&at24cxx))
    {
        shell_printf("at24cxx self test failed\r\n");
        return 1;  
    }
    shell_printf("at24cxx  test ok\r\n");
    return 0;
}

int DoI2C(int argc, char *const argv[])
{
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

