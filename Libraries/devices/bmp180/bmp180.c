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
    uint8_t data;
    device->read_reg(device, BMP180_CHIP_ID_REG, &data);
    if(data == 0x55)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}



uint8_t bmp180_read_temperature(bmp180_device * device, uint32_t * temperature)
{
    uint8_t reg1,reg2;
    uint16_t tem_data;
    device->read_reg(device, BMP180_CTRL_MEAS_REG, &reg1);
    if(reg1 & (1<<5)) //busy
    {
        return 1;
    }
    device->read_reg(device, BMP180_ADC_OUT_MSB_REG, &reg1);
    device->read_reg(device, BMP180_ADC_OUT_LSB_REG, &reg2);
    tem_data = (reg1<<8) | reg2;
    *temperature = tem_data;
    return 0;
}


uint8_t bmp180_start_temperature_conversion(bmp180_device * device)
{
    device->write_reg(device, BMP180_CTRL_MEAS_REG, BMP180_T_MEASURE);
}

uint8_t bmp180_init(bmp180_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate)
{
    device->name = name;
    device->device_addr = BMP180_ADDR;
    device->read_reg = bmp180_read_register;
    device->write_reg = bmp180_write_register;
    device->probe = bmp180_probe;
    device->read_temperature = bmp180_read_temperature;
    device->start_temperature_conversion = bmp180_start_temperature_conversion;
  //  device->read_data = hmc5883_read_data;
    
    // init i2c
    device->i2c_instance = I2C_QuickInit(I2CxMap, baudrate);
	// init sequence
  //  device->write_reg(device, HMC_CFG1, 0x78);
 //   device->write_reg(device, HMC_CFG2, 0x00);
  //  device->write_reg(device, HMC_MOD, 0x00);  
    return 0;
}


