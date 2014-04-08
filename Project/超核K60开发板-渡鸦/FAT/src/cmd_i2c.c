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
    struct i2c_device device;
    if(kinetis_i2c_bus_init(&bus, 0))
    {
        printf("i2c bus init failed\r\n");
    }
    device.config.baudrate = 100*1000;
    device.config.data_width = 8;
    device.subaddr_len = 1;
    device.subaddr = 0;
    i2c_bus_attach_device(&bus, &device);
    for(i=0;i<127;i++)
    {
        device.chip_addr = i;
        if(!i2c_probe(&device))
        {
            printf("ADDR:0x%2X(7BIT) | 0x%2X(8BIT) found!\r\n", i, i<<1);
        }
    }

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
    uint32_t ret;
    static struct i2c_bus bus;
    ret = kinetis_i2c_bus_init(&bus, HW_I2C0);
    if(ret)
    {
        printf("i2c bus init failed!\r\n");
    }
    
    ret = at24cxx_init(&bus, "at24c02");
    if(ret)
    {
        printf("at24cxx init failed!\r\n");
    }
    
    shell_printf("at24cxx size:%d btye\r\n", at24cxx_get_size());
    if(at24cxx_self_test())
    {
        shell_printf("at24cxx self test failed\r\n");
        return 1;  
    }
    printf("at24cxx test ok!\r\n");

}

static int DO_I2C_ADXL345(int argc, char *const argv[])
{
    short x,y,z;
    short ax, ay, az;
    uint32_t ret;
    static struct i2c_bus bus;
    ret = kinetis_i2c_bus_init(&bus, HW_I2C0);
    if(ret)
    {
        printf("i2c bus init failed!\r\n");
        return 1;
    }
    if(adxl345_init(&bus))
    {
        printf("adxl345 init failed\r\n");
        return 1;
    }
    if(adxl345_probe())
    {
        printf("adxl345 init failed\r\n");
        return 1;
    }     
    printf("device found:ID:0x%02X\r\n", adxl345_get_addr());
    adxl345_calibration();
    while(1)
    {
        if(!adxl345_readXYZ(&x, &y, &z))
        {
            adxl345_convert_angle(x, y, z, &ax, &ay, &az);
            printf("X:%4d Y:%4d Z:%4d AX:%4d AY:%4d AZ:%4d  \r", x, y, z, ax, ay ,az); 
        }
        DelayMs(5);
    }
    return 0;
   
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

