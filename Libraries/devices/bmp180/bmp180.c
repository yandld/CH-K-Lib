#include "bmp180.h"
#include <string.h>
#include "i2c.h"

uint8_t bmp180_read_register(bmp180_device *device, uint8_t reg_addr, uint8_t *data)
{
    return I2C_ReadSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}

//!< write register
uint8_t bmp180_write_register(bmp180_device * device, uint8_t reg_addr, uint8_t data)
{
    return I2C_WriteSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}


//!< proble hmc5883
uint8_t bmp180_probe(bmp180_device * device)
{
    I2C_GenerateSTART(device->i2c_instance);
    I2C_Send7bitAddress(device->i2c_instance, BMP180_ADDR, kI2C_Write);
    if(I2C_WaitAck(device->i2c_instance))
    {
        I2C_GenerateSTOP(device->i2c_instance);
        while(!I2C_IsBusy(device->i2c_instance));
        return 1;
    }
    else
    {
        I2C_GenerateSTOP(device->i2c_instance);
        while(!I2C_IsBusy(device->i2c_instance)); 
        return 0;
    }
}

uint8_t bmp180_init(bmp180_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate)
{
    device->name = name;
    
   // device->device_addr = HMC5883_SLAVE_ADDRESS;
    device->read_reg = bmp180_read_register;
    device->write_reg = bmp180_write_register;
    device->probe = bmp180_probe;
  //  device->read_data = hmc5883_read_data;
    
    // init i2c
    device->i2c_instance = I2C_QuickInit(I2CxMap, baudrate);
	// init sequence
  //  device->write_reg(device, HMC_CFG1, 0x78);
 //   device->write_reg(device, HMC_CFG2, 0x00);
  //  device->write_reg(device, HMC_MOD, 0x00);  
    return 0;
}


