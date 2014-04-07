#include "spi_abstraction.h"
#include "spi.h"
#include "gpio.h"

static int kinetis_spi_configure(spi_device_t device, struct spi_config *configuration)
{
    SPI_FrameFormat_Type mode;
    switch(configuration->mode & (SPI_CPHA | SPI_CPHA))
    {
        case SPI_MODE_0:
            mode = kSPI_CPOL0_CPHA0;
            break;
        case SPI_MODE_1:
            mode = kSPI_CPOL0_CPHA1;
            break;
        case SPI_MODE_2:
            mode = kSPI_CPOL1_CPHA0;
            break;
        case SPI_MODE_3:
            mode = kSPI_CPOL1_CPHA1;
            break;
        default:
            break;
    }
    SPI_FrameConfig(device->bus->instance, HW_CTAR0, mode, configuration->data_width, kSPI_MSBFirst, configuration->baudrate);
    return 0;
}

static int kinetis_spi_read(spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive)
{
    uint16_t dummy = 0xFFFF;
    len--;
    while(len--)
    {
        *buf++ = SPI_ReadWriteByte(device->bus->instance ,HW_CTAR0, dummy, device->csn, kspi_cs_keep_asserted);
    }
    if(!cs_return_inactive)
    {
        *buf++ = SPI_ReadWriteByte(device->bus->instance, HW_CTAR0, dummy, device->csn, kspi_cs_keep_asserted);
    }
    else
    {
        *buf++ = SPI_ReadWriteByte(device->bus->instance, HW_CTAR0, dummy, device->csn, kspi_cs_return_inactive);
    }
    return SPI_EOK;
}

static int kinetis_spi_write(spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive)
{
    len--;
    while(len--)
    {
        SPI_ReadWriteByte(device->bus->instance, HW_CTAR0, *buf++, device->csn, kspi_cs_keep_asserted);
    }
    if(!cs_return_inactive)
    {
        SPI_ReadWriteByte(device->bus->instance, HW_CTAR0, *buf++, device->csn, kspi_cs_keep_asserted);
    }
    else
    {
        SPI_ReadWriteByte(device->bus->instance, HW_CTAR0, *buf++, device->csn, kspi_cs_return_inactive);
    }
    return SPI_EOK;
}

const static struct spi_ops kinetis_spi_ops = 
{
    kinetis_spi_configure,
    kinetis_spi_read,
    kinetis_spi_write,
};

int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance)
{
    uint32_t _instnace;
    /* init hardware */
    _instnace = SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14, kSPI_CPOL0_CPHA1, 1000*1000);
    if(_instnace != instance)
    {
        return 1;
    }
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); //SPI2_PCS1
    /* register bus */
    bus->instance = instance;
    return spi_bus_register(bus, &kinetis_spi_ops);
}

