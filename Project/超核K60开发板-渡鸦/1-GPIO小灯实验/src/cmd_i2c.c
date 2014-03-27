#include "shell.h"
#include "i2c.h"
#include "board.h"
#include "common.h"
#include "at24cxx.h"
#include "adxl345.h"
#include "gpio.h"
#include "i2c_abstraction.h"
#include "systick.h"

static uint32_t gI2C_Instance;

static int DO_I2C_SCAN(int argc, char *const argv[])
{
    uint8_t i;
    struct i2c_bus bus;
    if(i2c_bus_init(&bus, BOARD_I2C_INSTANCE, 100*1000))
    {
        printf("i2c init failed\r\n");
        return 1;
    }
    shell_printf("i2c scanning bus at speed:%d\r\n", bus.baudrate);
    for(i = 1;i < 127; i++)
    {
        if(bus.probe(&bus, i) == ki2c_status_ok)
        {
            shell_printf("ADDR:0x%2X(7BIT) | 0x%2X(8BIT) found!\r\n", i, i<<1);
        }
    }
    return 0;
}

void I2C_ISR(void)
{
    shell_printf("enter i2c it\r\n");
}

static int DO_I2C_IT(int argc, char *const argv[])
{
    gI2C_Instance = I2C_QuickInit(BOARD_I2C_MAP, 47000);
    shell_printf("Scanning I2C%d bus at:%dHz\r\n",gI2C_Instance, 47000);
    uint8_t i2c_7bit_address = 0x44;
    shell_printf("i2c it test Address:0x%x(%d)\r\n", i2c_7bit_address, i2c_7bit_address);
    I2C_CallbackInstall(gI2C_Instance, I2C_ISR);
    I2C_ITDMAConfig(gI2C_Instance, kI2C_IT_BTC);
    I2C_GenerateSTART(gI2C_Instance);
    I2C_Send7bitAddress(gI2C_Instance, i2c_7bit_address, kI2C_Write);
    shell_printf("waitting for IT...\r\n");
    DelayMs(200);
    I2C_GenerateSTOP(gI2C_Instance);
    I2C_ITDMAConfig(gI2C_Instance, kI2C_IT_Disable);
    return 0;
}

static int DO_I2C_AT24CXX(int argc, char *const argv[])
{
    uint32_t size;
    struct i2c_bus bus = {0};
    struct at24cxx_device at24cxx = {0};
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

static int DO_I2C_ADXL345(int argc, char *const argv[])
{
    short x,y,z;
    short ax, ay, az;
    struct i2c_bus bus = {0};
    struct adxl345_device adxl345 = {0};
    if(i2c_bus_init(&bus, BOARD_I2C_INSTANCE, 40*1000))
    {
        shell_printf("i2c bus init failed\r\n");
    }
    adxl345.bus = &bus;
    if(adxl345_init(&adxl345, 0x53))
    {
        shell_printf("init at24cxx failed\r\n");
        return 1;
    }
    if(adxl345.probe(&adxl345))
    {
        shell_printf("no device found\r\n");
        return 1;
    }
    adxl345.calibration(&adxl345);
    while(1)
    {
        if(!adxl345.readXYZ(&adxl345, &x, &y, &z))
        {
            adxl345.convert_angle(x, y, z, &ax, &ay, &az);
            printf("X:%4d Y:%4d Z:%4d AX:%4d AY:%4d AZ:%4d  \r", x, y, z, ax, ay ,az); 
        }
    }
}


int CMD_I2C(int argc, char *const argv[])
{
    PORT_PinMuxConfig(HW_GPIOB, 2, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOB, 3, kPinAlt2);
    PORT_PinOpenDrainConfig(HW_GPIOB, 2, ENABLE); 
    PORT_PinOpenDrainConfig(HW_GPIOB, 3, ENABLE);
    PORT_PinPullConfig(HW_GPIOB, 3, kPullUp);
    PORT_PinPullConfig(HW_GPIOB, 2, kPullUp);
    
    if((argc == 2) && (!strcmp(argv[1], "SCAN")))
    {
        return DO_I2C_SCAN(argc, argv);
    }
    if((argc == 2) && (!strcmp(argv[1], "IT")))
    {
        return DO_I2C_IT(argc, argv);
    }
    if((argc == 2) && (!strcmp(argv[1], "AT24CXX")))
    {
        return DO_I2C_AT24CXX(argc, argv);
    }
    if((argc == 2) && (!strcmp(argv[1], "ADXL345")))
    {
        return DO_I2C_ADXL345(argc, argv);
    }
    return CMD_RET_USAGE;
}



const cmd_tbl_t CommandFun_I2C = 
{
    .name = "I2C",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = CMD_I2C,
    .usage = "I2C <CMD> (CMD = SCAN,IT,AT24CXX,ADXL345)",
    .complete = NULL,
    .help = "I2C <CMD> (CMD = SCAN,IT,AT24CXX,ADXL345)",

};

