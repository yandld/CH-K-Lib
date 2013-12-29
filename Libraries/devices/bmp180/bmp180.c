#include "bmp180.h"
#include <string.h>
#include "i2c.h"

//!< Calibration parameters structure
typedef struct
{
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
    int16_t B5;
	int16_t MB;
    int16_t M2C;
	int16_t MD;
    uint8_t oss;
    int32_t pressure;
    int32_t temperature;
} bmp180_data_t;

static bmp180_data_t bmp_data1;

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

//!< Read the calibration data.
static uint8_t dump_calibration_data(bmp180_device * device)
{
	uint8_t err = 0;
	uint8_t data1,data2;
    err  = device->read_reg(device, BMP180_REG_AC1, &data1);
    err += device->read_reg(device, BMP180_REG_AC1+1, &data2);
    bmp_data1.AC1 = (data1<<8)+data2;
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_AC2, &data1);
        err += device->read_reg(device, BMP180_REG_AC2+1, &data2);
        bmp_data1.AC2 = (data1<<8)+data2;
    }
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_AC3, &data1);
        err += device->read_reg(device, BMP180_REG_AC3+1, &data2);
        bmp_data1.AC3 = (data1<<8)+data2;
    } 
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_AC4, &data1);
        err += device->read_reg(device, BMP180_REG_AC4+1, &data2);
        bmp_data1.AC4 = (data1<<8)+data2;
    } 
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_AC5, &data1);
        err += device->read_reg(device, BMP180_REG_AC5+1, &data2);
        bmp_data1.AC5 = (data1<<8)+data2;
    } 
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_AC6, &data1);
        err += device->read_reg(device, BMP180_REG_AC6+1, &data2);
        bmp_data1.AC6 = (data1<<8)+data2;
    } 
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_B1, &data1);
        err += device->read_reg(device, BMP180_REG_B1+1, &data2);
        bmp_data1.B1 = (data1<<8)+data2;
    } 
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_B2, &data1);
        err += device->read_reg(device, BMP180_REG_B2+1, &data2);
        bmp_data1.B2 = (data1<<8)+data2;
    }  
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_MB, &data1);
        err += device->read_reg(device, BMP180_REG_MB+1, &data2);
        bmp_data1.MB = (data1<<8)+data2;
    }  
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_MC, &data1);
        err += device->read_reg(device, BMP180_REG_MC+1, &data2);
        bmp_data1.M2C = (data1<<8)+data2;
    }  
    if(!err)
    {
        err  = device->read_reg(device, BMP180_REG_MD, &data1);
        err += device->read_reg(device, BMP180_REG_MD+1, &data2);
        bmp_data1.MD = (data1<<8)+data2;
    }  
	return(err);
}


//!< read temp
uint8_t bmp180_read_temperature(bmp180_device * device, int32_t * temperature)
{
    uint8_t reg1,reg2;
    uint16_t tem_data;
	int32_t x1, x2;
    bmp180_data_t* bmp_data = device->bmp_data;
    device->read_reg(device, BMP180_CTRL_MEAS_REG, &reg1);
    if(reg1 & (1<<5)) //busy
    {
        return 1;
    }
    device->read_reg(device, BMP180_ADC_OUT_MSB_REG, &reg1);
    device->read_reg(device, BMP180_ADC_OUT_LSB_REG, &reg2);
    tem_data = (reg1<<8) | reg2;
    
	x1 = (tem_data - bmp_data->AC6) * bmp_data->AC5 >> 15;
	x2 = ((int32_t)bmp_data->M2C << 11) / (x1 + bmp_data->MD);
    *temperature = ((x1 + x2) + 8) >> 4;
    bmp_data->temperature = *temperature;
    return 0;
}

static void math_pressure(bmp180_device * device, uint32_t raw_p)
{
	uint32_t b4, b7;
	int32_t x1, x2, x3, b3, b6;
    bmp180_data_t* bmp_data = device->bmp_data;
	b6 = bmp_data->B5 - 4000;
	x1 = (bmp_data->B2 * (b6 * b6 >> 12)) >> 11;
	x2 = bmp_data->AC2 * b6 >> 11;
	x3 = x1 + x2;

	b3 = ((((int32_t)bmp_data->AC1 * 4 + x3) << bmp_data->oss) + 2) >> 2;
	x1 = bmp_data->AC3 * b6 >> 13;
	x2 = (bmp_data->B1 * (b6 * b6 >> 12)) >> 16;
	x3 = (x1 + x2 + 2) >> 2;
	b4 = (bmp_data->AC4 * (uint32_t)(x3 + 32768)) >> 15;
	b7 = (uint32_t)(raw_p - b3) * (50000 >> bmp_data->oss);

	if (b7 < 0x80000000)
		bmp_data->pressure = (b7 << 1) / b4;
	else
		bmp_data->pressure = (b7 / b4) << 1;

	x1 = (bmp_data->pressure >> 8) * (bmp_data->pressure >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * bmp_data->pressure) >> 16;
	bmp_data->pressure += ((x1 + x2 + 3791) >> 4);
}

//!< read pressure
uint8_t bmp180_read_pressure(bmp180_device * device, int32_t * pressure)
{
    uint8_t reg1,reg2,reg3;
    uint32_t tem_data;
    bmp180_data_t* bmp_data = device->bmp_data;
    device->read_reg(device, BMP180_CTRL_MEAS_REG, &reg1);
    if(reg1 & (1<<5)) //busy
    {
        return 1;
    }
    bmp_data->oss = reg1>>6;
    device->read_reg(device, BMP180_ADC_OUT_MSB_REG, &reg1);
    device->read_reg(device, BMP180_ADC_OUT_LSB_REG, &reg2);
    device->read_reg(device, BMP180_ADC_OUT_XLSB_REG, &reg3);
    //read oss
    tem_data = (reg1<<16) + (reg2<<8) + reg3;
    tem_data >>= (8 - bmp_data->oss);
    math_pressure(device, tem_data);
    *pressure = bmp_data->pressure;
    *pressure = tem_data;
    return 0;
}

//!< start pressure conversion
uint8_t bmp_start_conversion(bmp180_device * device, uint8_t mode)
{
    return device->write_reg(device, BMP180_CTRL_MEAS_REG, mode); 
}


//!< init
uint8_t bmp180_init(bmp180_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate)
{
    uint8_t err;
    static uint8_t init = 0;
    device->name = name;
    device->device_addr = BMP180_ADDR;
    device->read_reg = bmp180_read_register;
    device->write_reg = bmp180_write_register;
    device->probe = bmp180_probe;
    device->read_temperature = bmp180_read_temperature;
    device->read_pressure = bmp180_read_pressure;
    
    device->start_conversion = bmp_start_conversion;
    device->bmp_data = (bmp180_data_t*)&bmp_data1;
    
    // init i2c
    if(!init)
    {
        device->i2c_instance = I2C_QuickInit(I2CxMap, baudrate);
        // init sequence
        init = 1;
    }
    err = dump_calibration_data(device);  
    return err;
}


