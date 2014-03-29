#include <rthw.h>
#include <rtthread.h>
#include "spi.h"
#include "gpio.h"

#include <rtdevice.h>
#include <drivers/spi.h>

static struct rt_spi_bus kinetis_spi;

static struct rt_spi_ops kinetis_spi_ops =
{
    RT_NULL,
    RT_NULL
};

int rt_hw_spi_init(void)
{
 //   return rt_spi_bus_register(&kinetis_spi, "spi", &kinetis_spi_ops); 
}







