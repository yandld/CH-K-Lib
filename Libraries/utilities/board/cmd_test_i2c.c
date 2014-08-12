#include "shell.h"
#include "gpio.h"
#include "i2c.h"
#include "board.h"


/* i2c bus scan */
static void I2C_Scan(uint32_t instance)
{
    uint8_t i;
    uint8_t ret;
    for(i = 1; i < 127; i++)
    {
        ret = I2C_BurstWrite(instance , i, 0, 0, NULL, 0);
        if(!ret)
        {
            printf("ADDR:0x%2X(7BIT) | 0x%2X(8BIT) found!\r\n", i, i<<1);
        }
    }
}

static int DoI2C(int argc, char * const argv[])
{
    uint32_t instance;
    printf("board:%s fun:%s\r\n",BOARD_NAME, __func__);
    /* init i2c */
    instance = I2C_QuickInit(BOARD_I2C_MAP, 100*1000);
    printf("i2c instance:%d\r\n", instance);
    I2C_Scan(instance);
}

SHELL_EXPORT_CMD(DoI2C, i2c , i2c test)
    

