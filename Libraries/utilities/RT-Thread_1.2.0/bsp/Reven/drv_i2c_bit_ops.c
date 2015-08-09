#include <rtthread.h>
#include "gpio.h"
#include "common.h"
#include <rtdevice.h>
#include <drivers/i2c.h>
#include "board.h"

#define I2C_PORT    BOARD_I2C_SDA_PORT
#define I2C_SDA     BOARD_I2C_SDA_PIN
#define I2C_SCL     BOARD_I2C_SCL_PIN

static void (set_sda)(void *data, rt_int32_t state)
{
    GPIO_PinConfig(I2C_PORT, I2C_SDA, kOutput);
    GPIO_WriteBit(I2C_PORT, I2C_SDA, state);
}

static void (set_scl)(void *data, rt_int32_t state)
{
    GPIO_PinConfig(I2C_PORT, I2C_SCL, kOutput);
    GPIO_WriteBit(I2C_PORT, I2C_SCL, state);
}

static rt_int32_t (get_sda)(void *data)
{
    GPIO_PinConfig(I2C_PORT, I2C_SDA, kInput);
    return GPIO_ReadBit(I2C_PORT, I2C_SDA);
}

static rt_int32_t (get_scl)(void *data)
{
    GPIO_PinConfig(I2C_PORT, I2C_SCL, kInput);
    return GPIO_ReadBit(I2C_PORT, I2C_SCL);
}

static void (udelay)(rt_uint32_t us)
{
    volatile int i,j;
    for(i=0;i<us;i++)
    {
        __NOP();
    }
}
    
static struct rt_i2c_bus_device i2c_bus;
static const struct rt_i2c_bit_ops bit_ops = 
{
    RT_NULL,
    set_sda,
    set_scl,
    get_sda,
    get_scl,
    udelay,
    1,
    10,
};

int rt_hw_i2c_bit_ops_bus_init(const char *name)
{
    rt_memset((void *)&i2c_bus, 0, sizeof(struct rt_i2c_bus_device));
    i2c_bus.priv = (void *)&bit_ops;
    
    /* init i2c gpio */
    GPIO_WriteBit(I2C_PORT, I2C_SDA, 1);
    GPIO_WriteBit(I2C_PORT, I2C_SCL, 1);
    GPIO_QuickInit(I2C_PORT, I2C_SDA, kGPIO_Mode_OPP);
    GPIO_QuickInit(I2C_PORT, I2C_SCL, kGPIO_Mode_OPP);

    PORT_PinPullConfig(I2C_PORT, I2C_SDA, kPullUp);
    PORT_PinPullConfig(I2C_PORT, I2C_SCL, kPullUp);

    return rt_i2c_bit_add_bus(&i2c_bus, name);
}


INIT_APP_EXPORT(rt_hw_i2c_bit_ops_bus_init);
