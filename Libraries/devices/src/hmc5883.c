#include "hmc5883.h"
#include <string.h>
#include "i2c.h"



uint8_t hmc5883_read_register(hmc5883_device *device, uint8_t reg_addr, uint8_t *data)
{
    return I2C_ReadSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}

//!< write register
uint8_t hmc5883_write_register(hmc5883_device * device, uint8_t reg_addr, uint8_t data)
{
    return I2C_WriteSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}

//!< proble hmc5883
uint8_t hmc5883_probe(hmc5883_device * device)
{
    I2C_GenerateSTART(device->i2c_instance);
    I2C_Send7bitAddress(device->i2c_instance, HMC5883_SLAVE_ADDRESS, kI2C_Write);
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



uint8_t hmc5883_read_data(hmc5883_device * device, int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t data1,data2;
    device->read_reg(device, HMC_DX_MSB, &data1);
    device->read_reg(device, HMC_DX_LSB, &data2);
    *x = (int16_t)((data1<<8) | data2);
    device->read_reg(device, HMC_DY_MSB, &data1);
    device->read_reg(device, HMC_DY_LSB, &data2);
    *y = (int16_t)((data1<<8) | data2);
    device->read_reg(device, HMC_DZ_MSB, &data1);
    device->read_reg(device, HMC_DZ_LSB, &data2);
    *z = (int16_t)((data1<<8) | data2);
    return 0;    
}

uint8_t hmc5883_init(hmc5883_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate)
{
    device->name = name;
    
    device->device_addr = HMC5883_SLAVE_ADDRESS;
    device->read_reg = hmc5883_read_register;
    device->write_reg = hmc5883_write_register;
    device->probe = hmc5883_probe;
    device->read_data = hmc5883_read_data;
    
    // init i2c
    device->i2c_instance = I2C_QuickInit(I2CxMap, baudrate);
	// init sequence
    device->write_reg(device, HMC_CFG1, 0x78);
    device->write_reg(device, HMC_CFG2, 0x00);
    device->write_reg(device, HMC_MOD, 0x00);  
    return 0;
}













