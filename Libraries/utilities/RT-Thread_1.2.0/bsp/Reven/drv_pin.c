#include "gpio.h"
#include "pin.h"
#include "finsh.h"


static void _pin_mode(struct rt_device *device, rt_base_t pin, rt_base_t mode)
{
    uint32_t instance;
    instance = (pin % 0xFF00) >> 8;
    switch(mode)
    {
        case PIN_MODE_OUTPUT:
            GPIO_QuickInit(instance, pin & 0xFF, kGPIO_Mode_OPP);
            break;
        case PIN_MODE_INPUT:
            GPIO_QuickInit(instance, pin & 0xFF, kGPIO_Mode_IFT);
            break;
        case PIN_MODE_INPUT_PULLUP:
            GPIO_QuickInit(instance, pin & 0xFF, kGPIO_Mode_IPU);
            break;
    }
}

static void _pin_write(struct rt_device *device, rt_base_t pin, rt_base_t value)
{
    uint32_t instance;
    instance = (pin % 0xFF00) >> 8;
    GPIO_WriteBit(instance, pin&0xFF, value);
}

static int _pin_read(struct rt_device *device, rt_base_t pin)
{
    uint32_t instance;
    instance = (pin % 0xFF00) >> 8;
    return GPIO_ReadBit(instance, pin & 0xFF);
}
    
static const struct rt_pin_ops _ops =
{
    _pin_mode,
    _pin_write,
    _pin_read,
};

int rt_hw_pin_init(const char *name)
{    
    return rt_device_pin_register(name, &_ops, RT_NULL);
}






