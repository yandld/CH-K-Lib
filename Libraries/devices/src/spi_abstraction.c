#include "spi_abstraction.h"
#include "spi.h"



spi_status spi_bus_deinit(spi_bus_t bus)
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
        SPI_InitTypeDef SPI_InitStruct1;
        SPI_InitStruct1.baudrate = bus->baudrate;
        SPI_InitStruct1.frameFormat = (SPI_FrameFormat_Type)bus->frame_type;
        SPI_InitStruct1.dataSize = 8;
        SPI_InitStruct1.instance = bus->instance;
        SPI_InitStruct1.mode = kSPI_Master;
        SPI_InitStruct1.bitOrder = kSPI_MSBFirst;
        SPI_InitStruct1.ctar = HW_CTAR0;
        SPI_Init(&SPI_InitStruct1);
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

spi_status bus_config(spi_bus_t bus, uint32_t bus_chl, spi_frame_type frame_type, uint32_t baudrate)
{
    if(!bus)
    {
        return kspi_status_error;
    }
    /* config frame format */
    SPI_FrameConfig(bus->instance, bus_chl, (SPI_FrameFormat_Type)frame_type, 8, kSPI_MSBFirst, baudrate);
    return kspi_status_ok;
}

spi_status spi_bus_read(spi_bus_t bus, spi_device_t device, uint8_t *buf, uint32_t len, bool is_return_inactive)
{
    uint16_t dummy = 0xFFFF;
    if(kspi_cs_keep_asserted == device->cs_state)
    {
        len--;
        while(len--)
        {
            *buf++ = SPI_ReadWriteByte(bus->instance, device->bus_chl, dummy, device->csn, kspi_cs_keep_asserted);
        }
        if(!is_return_inactive)
        {
            *buf++ = SPI_ReadWriteByte(bus->instance, device->bus_chl, dummy, device->csn, kspi_cs_keep_asserted);
        }
        else
        {
            *buf++ = SPI_ReadWriteByte(bus->instance, device->bus_chl, dummy, device->csn, kspi_cs_return_inactive);
        }
    }
    else
    {
        while(len--)
        {
            *buf++ = SPI_ReadWriteByte(bus->instance, device->bus_chl, dummy, device->csn, kspi_cs_return_inactive);
        }
    }
    return kspi_status_ok;
}

spi_status spi_bus_write(spi_bus_t bus, spi_device_t device, uint8_t *buf, uint32_t len, bool cs_return_inactive)
{
    if(kspi_cs_keep_asserted == device->cs_state)
    {
        len--;
        while(len--)
        {
            SPI_ReadWriteByte(bus->instance, device->bus_chl, *buf++, device->csn, kspi_cs_keep_asserted);
        }
        if(!cs_return_inactive)
        {
            SPI_ReadWriteByte(bus->instance, device->bus_chl, *buf++, device->csn, kspi_cs_keep_asserted);
        }
        else
        {
            SPI_ReadWriteByte(bus->instance, device->bus_chl, *buf++, device->csn, kspi_cs_return_inactive);
        }
    }
    else
    {
        while(len--)
        {
            SPI_ReadWriteByte(bus->instance, device->bus_chl, *buf++, device->csn, kspi_cs_return_inactive);
        }
    }
    return kspi_status_ok;
}


