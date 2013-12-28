#include "mpu6050.h"
#include <string.h>
//!< read regiser
uint8_t mpu6050_read_register(mpu6050_device *device, uint8_t reg_addr, uint8_t *data)
{
    return I2C_ReadSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}

//!< write register
uint8_t mpu6050_write_register(mpu6050_device * device, uint8_t reg_addr, uint8_t data)
{
    return I2C_WriteSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}
//!< read accelator data
uint8_t mpu6050_read_accel(mpu6050_device * device, int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t data1,data2;
    device->read_reg(device, ACCEL_XOUT_H, &data1);
    device->read_reg(device, ACCEL_XOUT_L, &data2);
    *x = (int16_t)((data1<<8) | data2);
    device->read_reg(device, ACCEL_YOUT_H, &data1);
    device->read_reg(device, ACCEL_YOUT_L, &data2);
    *y = (int16_t)((data1<<8) | data2);
    device->read_reg(device, ACCEL_ZOUT_H, &data1);
    device->read_reg(device, ACCEL_ZOUT_L, &data2);
    *z = (int16_t)((data1<<8) | data2);
    return 0;    
}

uint8_t mpu6050_crtl(mpu6050_device * device, uint8_t cmd)
{
    
    return 0;
}

//!< read gyro data
uint8_t mpu6050_read_gyro(mpu6050_device * device, int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t data1,data2;
    device->read_reg(device, GYRO_XOUT_H, &data1);
    device->read_reg(device, GYRO_XOUT_L, &data2);
    *x = (int16_t)((data1<<8) | data2);
    device->read_reg(device, GYRO_YOUT_H, &data1);
    device->read_reg(device, GYRO_YOUT_L, &data2);
    *y = (int16_t)((data1<<8) | data2);
    device->read_reg(device, GYRO_ZOUT_H, &data1);
    device->read_reg(device, GYRO_ZOUT_L, &data2);
    *z = (int16_t)((data1<<8) | data2);
    return 0;    
}






//!< proble
#define WHO_AM_I_VALUE  (0x68)
uint8_t mpu6050_probe(mpu6050_device * device)
{
    uint8_t data;
    if(device->read_reg(device, WHO_AM_I, &data))
    {
        return 2;
    }
    if(data == WHO_AM_I_VALUE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t mpu6050_init(mpu6050_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate)
{
    device->name = name;
    device->device_addr = MPU6050_DEVICE_ADDR;
    device->read_reg = mpu6050_read_register;
    device->write_reg = mpu6050_write_register;
    device->probe = mpu6050_probe;
    device->read_accel = mpu6050_read_accel;
    device->read_gyro = mpu6050_read_gyro;
    device->ctrl = mpu6050_crtl;
    // init i2c
    device->i2c_instance = I2C_QuickInit(I2CxMap, baudrate);
	// init sequence
	device->write_reg(device, PWR_MGMT_1 , 0x00 );
	device->write_reg(device, SMPLRT_DIV , 0x07 );
	device->write_reg(device, CONFIG , 0x07 );
	device->write_reg(device, AUX_VDDIO,0x80);
	device->write_reg(device, GYRO_CONFIG , 0x18 );
	device->write_reg(device, ACCEL_CONFIG , 0x01 );
	device->write_reg(device, I2C_MST_CTRL,0x00);
	device->write_reg(device, INT_PIN_CFG,0x02);
    
    return 0;
}


