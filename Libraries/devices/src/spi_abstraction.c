#include "spi_abstraction.h"
#include "board.h"
#include "spi.h"


static uint32_t g_instance;

spi_status spi_bus_deinit(struct spi_bus * bus)
{
    return kspi_status_unsupported;
}

spi_status spi_bus_init(struct spi_bus * bus, uint32_t instance)
{
    static uint8_t bus_open_flag = 0;
    if(!bus)
    {
        return kspi_status_error;
    }
    bus->baudrate = 10*1000; //inital baudrate
    bus->instance = instance;
    
    if(!bus_open_flag)
    {
        g_instance = SPI_QuickInit(BOARD_SPI_MAP, kSPI_CPOL0_CPHA1, bus->baudrate);
        bus_open_flag = 1;
    }
    // link ops
    bus->init = spi_bus_init;
    bus->deinit = spi_bus_deinit;
    bus->read = spi_bus_read;
    bus->write = spi_bus_write;
    bus->bus_config = bus_config;
    return kspi_status_ok;
}

spi_status bus_config(struct spi_bus * bus, uint32_t bus_chl, spi_frame_type frame_type, uint32_t baudrate)
{
    if(!bus)
    {
        return kspi_status_error;
    }
    // config frame format
    SPI_FrameConfig(bus->instance, bus_chl, (SPI_FrameFormat_Type)frame_type, 8, kSPI_MSBFirst, baudrate);
    return kspi_status_ok;
}

spi_status spi_bus_read(struct spi_bus * bus, spi_device * device, uint8_t *buf, uint32_t len, bool is_return_inactive)
{
    uint16_t dummy = 0xFFFF;
    if(kspi_cs_keep_asserted == device->cs_state)
    {
        len--;
        while(len--)
        {
            *buf++ = SPI_ReadWriteByte(g_instance, device->bus_chl, dummy, device->csn, kspi_cs_keep_asserted);
        }
        if(!is_return_inactive)
        {
            *buf++ = SPI_ReadWriteByte(g_instance, device->bus_chl, dummy, device->csn, kspi_cs_keep_asserted);
        }
        else
        {
            *buf++ = SPI_ReadWriteByte(g_instance, device->bus_chl, dummy, device->csn, kspi_cs_return_inactive);
        }
    }
    else
    {
        while(len--)
        {
            *buf++ = SPI_ReadWriteByte(g_instance, device->bus_chl, dummy, device->csn, kspi_cs_return_inactive);
        }
    }
    return kspi_status_ok;
}

spi_status spi_bus_write(struct spi_bus * bus, spi_device * device, uint8_t *buf, uint32_t len, bool cs_return_inactive)
{
    if(kspi_cs_keep_asserted == device->cs_state)
    {
        len--;
        while(len--)
        {
            SPI_ReadWriteByte(g_instance, device->bus_chl, *buf++, device->csn, kspi_cs_keep_asserted);
        }
        if(!cs_return_inactive)
        {
            SPI_ReadWriteByte(g_instance, device->bus_chl, *buf++, device->csn, kspi_cs_keep_asserted);
        }
        else
        {
            SPI_ReadWriteByte(g_instance, device->bus_chl, *buf++, device->csn, kspi_cs_return_inactive);
        }
    }
    else
    {
        while(len--)
        {
            SPI_ReadWriteByte(g_instance, device->bus_chl, *buf++, device->csn, kspi_cs_return_inactive);
        }
    }
    return kspi_status_ok;
}


