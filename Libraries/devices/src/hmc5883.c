/**
  ******************************************************************************
  * @file    hmc5883.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "hmc5883.h"
#include <string.h>

#define HMC_CFG1		0x00		//Register ConfigA
#define HMC_CFG2		0x01		//Register ConfigB	
#define HMC_MOD			0x02		//Mode Register
#define HMC_DX_MSB		0x03		//data output x	MSB 
#define HMC_DX_LSB		0x04		//data output x LSB
#define HMC_DY_MSB		0x05		//data output y MSB
#define HMC_DY_LSB		0x06		//data output y LSB
#define HMC_DZ_MSB		0x07		//data output z MSB
#define HMC_DZ_LSB		0x08		//data output z LSB
#define HMC_STATUE		0x09		//Statue Register
#define HMC_IDTF_A		0x0A		//Identification Register A
#define HMC_IDTF_B		0x0B		//Identification Register B
#define HMC_IDTF_C		0x0C		//Identification Register C

static struct i2c_device device;
static const uint8_t chip_addr_table[] = {0x1E};


int hmc5883_init(struct i2c_bus* bus)
{
    uint32_t ret = 0;
    /* i2c bus config */
    device.config.baudrate = 200*1000;
    device.config.data_width = 8;
    device.config.mode = 0;
    device.subaddr_len = 1;
    /* attach device to bus */
    ret = i2c_bus_attach_device(bus, &device);
    return ret;
}

static int write_register(uint8_t addr, uint8_t value)
{
    uint8_t buf[1];
    device.subaddr = addr;
    buf[0] = value;
    if(device.bus->ops->write(&device, buf, 1))
    {
        return 1;
    }
    return 0;
}

int hmc5883_probe(void)
{
    uint32_t i;
    uint8_t buf[3];
    device.subaddr = HMC_IDTF_A;
    for(i=0;i<ARRAY_SIZE(chip_addr_table);i++)
    {
        device.chip_addr = chip_addr_table[i];
        if(device.bus->ops->read(&device, buf, sizeof(buf)) == I2C_EOK)
        {
            /* find the device */
            device.chip_addr = chip_addr_table[i];
            /* ID match */
            if(!memcmp(buf, "H43", 3))
            {
                #if LIB_DEBUG
                printf("hmc5883 found!\r\n");
                #endif
                /* init sequence */
                write_register(HMC_CFG1, 0x78);
                write_register(HMC_CFG2, 0x00);
                write_register(HMC_MOD, 0x00);  
                return 0; 
            }
        }
    }
    return 1; 
}

int mpu6050_read_data(int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t buf[6];
    device.subaddr = HMC_DX_MSB;
    if(device.bus->ops->read(&device,buf, 6))
    {
        return 1;
    }
    *x=(int16_t)(((uint16_t)buf[0]<<8)+buf[1]); 	    
    *y=(int16_t)(((uint16_t)buf[2]<<8)+buf[3]); 	    
    *z=(int16_t)(((uint16_t)buf[4]<<8)+buf[5]); 
    return 0;    
}
