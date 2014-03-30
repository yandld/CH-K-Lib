#include "bmp180.h"
#include <string.h>
#include "common.h"
#include "i2c.h"

static uint8_t bmp180_read_register(bmp180_device *device, uint8_t reg_addr, uint8_t *data)
{
    return I2C_ReadSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}

//!< write register
static uint8_t bmp180_write_register(bmp180_device * device, uint8_t reg_addr, uint8_t data)
{
    return I2C_WriteSingleRegister(device->i2c_instance, device->device_addr, reg_addr, data);
}

/*!
 * @brief start bmp180 conversion
 *
 * @param pointer of the bmp180 device struct
 * @param conversion mode: 
 *        @arg BMP180_T_MEASURE: temperature conversion
 *        @arg BMP180_P0_MEASURE: pressure conversion, oversampling:0
 *        @arg BMP180_P1_MEASURE: pressure conversion, oversampling:1
 *        @arg BMP180_P2_MEASURE: pressure conversion, oversampling:2
 */
int32_t bmp180_start_conversion(bmp180_device * device, uint8_t mode)
{
    return bmp180_write_register(device, BMP180_CTRL_MEAS_REG, mode); 
}

/*!
 * @brief read raw temperature data
 *
 * @param pointer of the bmp180 device struct
 * @param data pointer
 */
static int32_t read_raw_temperature(bmp180_device * device, int32_t * data)
{
    uint8_t reg1,reg2;
    uint8_t ret = 0;
    ret += bmp180_read_register(device, BMP180_ADC_OUT_MSB_REG, &reg1);
    ret += bmp180_read_register(device, BMP180_ADC_OUT_LSB_REG, &reg2);
    *data = (reg1<<8) | reg2;
    return ret;
}

/*!
 * @brief read raw perssure data
 *
 * @param pointer of the bmp180 device struct
 * @param data pointer
 * @param oversampling rate data pointer
 */
static int32_t read_raw_pressure(bmp180_device * device, int32_t * data, uint8_t *oss)
{
    uint8_t reg1,reg2,reg3;
    uint8_t ret = 0;
    /* read oss */
    ret += bmp180_read_register(device, BMP180_CTRL_MEAS_REG, &reg1);
    *oss = reg1 >> 6;
    ret += bmp180_read_register(device, BMP180_ADC_OUT_MSB_REG, &reg1);
    ret += bmp180_read_register(device, BMP180_ADC_OUT_LSB_REG, &reg2);
    ret += bmp180_read_register(device, BMP180_ADC_OUT_XLSB_REG, &reg3);
    
    *data = (reg1 << 16) + (reg2 << 8) + reg3;
    *data >>= (8 - (*oss));
    return ret;
}

/*!
 * @brief tell if bmp180 is in conversion
 *
 * @param pointer of the bmp180 device struct
 * @retval 1:busy 0:idle
 */
static int32_t is_conversion_busy(bmp180_device * device)
{
    uint8_t reg1;
    bmp180_read_register(device, BMP180_CTRL_MEAS_REG, &reg1);
    if(reg1 & (1 << 5)) /* busy */
    {
        return 1;
    }
    return 0;
}


/*!
 * @brief read real pressure data
 *
 * calculate according to bmp180 data sheet
 *
 * @param pointer of the bmp180 device struct
 * @param pointer of pressure, in pa
 */
int32_t bmp180_read_pressure(bmp180_device * device, int32_t * pressure)
{
    int32_t raw_pressure;
    uint8_t oss;
    bmp180_calibration_data_t* cal_data = &device->data;
    if(is_conversion_busy(device))
    {
        return 1;
    }
    /* read raw pressure */
    if(read_raw_pressure(device, &raw_pressure, &oss))
    {
        return 2;
    }
    /* temperature compensation */
    uint32_t b4, b7;
    int32_t x1, x2, x3, b3, b6;
    int32_t result;
    b6 = cal_data->B5 - 4000;
        
    x1 = (cal_data->B2 * (b6 * b6 >> 12)) >> 11;
    x2 = cal_data->AC2 * b6 >> 11;
    x3 = x1 + x2;

    b3 = ((((int32_t)cal_data->AC1 * 4 + x3) << oss) + 2) >> 2;
    x1 = cal_data->AC3 * b6 >> 13;
    x2 = (cal_data->B1 * (b6 * b6 >> 12)) >> 16;
    x3 = (x1 + x2 + 2) >> 2;
    b4 = (cal_data->AC4 * (uint32_t)(x3 + 32768)) >> 15;
    b7 = (uint32_t)(raw_pressure - b3) * (50000 >> oss);

    if (b7 < 0x80000000)
    {
        result = (b7 << 1) / b4;
    }
    else
    {
        result = (b7 / b4) << 1;
    }
    x1 = (result >> 8) * (result >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * result) >> 16;
    result += ((x1 + x2 + 3791) >> 4);
    *pressure = result;
    return 0;
}


/*!
 * @brief read real temperature data
 *
 * calculate according to bmp180 data sheet
 *
 * @param pointer of the bmp180 device struct
 * @param pointer of temperature, in 10x, eg:156 = 15.6C
 */
int32_t bmp180_read_temperature(bmp180_device * device, int32_t * temperature)
{
    int32_t raw_temperature;
    int32_t x1, x2;
    bmp180_calibration_data_t* cal_data = &device->data;
    if(is_conversion_busy(device))
    {
        return 1;
    }
    if(read_raw_temperature(device, &raw_temperature))
    {
        return 2;
    }
    x1 = (raw_temperature - cal_data->AC6) * cal_data->AC5 >> 15;
    x2 = ((int32_t)cal_data->MC << 11) / (x1 + cal_data->MD);
    cal_data->B5 = x1 + x2;
    *temperature = ((x1 + x2) + 8) >> 4;
    return 0;
}

/*!
 * @brief dump bmp180 calibration data from bmp180 internal eeporm to struct
 *
 * @param pointer of the bmp180 device struct
 */
static int32_t dump_calibration_data(bmp180_device * device)
{
    uint8_t ret = 0;
    ret = I2C_BurstRead(device->i2c_instance, device->device_addr, BMP180_PROM_START_ADDR, 1, (uint8_t*)&device->data, BMP180_PROM_DATA_LEN);
    if(ret)
    {
      return 1;
    }
    device->data.AC1 = __REV16(device->data.AC1);
    device->data.AC2 = __REV16(device->data.AC2);
    device->data.AC3 = __REV16(device->data.AC3);
    device->data.AC4 = __REV16(device->data.AC4);
    device->data.AC5 = __REV16(device->data.AC5);
    device->data.AC6 = __REV16(device->data.AC6);
    device->data.B1 = __REV16(device->data.B1);
    device->data.B2 = __REV16(device->data.B2);
    device->data.MB = __REV16(device->data.MB);
    device->data.MC = __REV16(device->data.MC);
    device->data.MD = __REV16(device->data.MD);
    return 0;
}

int32_t bmp180_init(bmp180_device* device, uint32_t instance, uint8_t deviceAddress, uint32_t baudrate)
{
    uint8_t data;
    if(!device)
    {
        return -1;
    }
    device->i2c_instance = instance;
    device->baudrate = baudrate;
    device->device_addr = deviceAddress;
    if(bmp180_read_register(device, BMP180_CHIP_ID_REG, &data))
    {
        return 1;
    }
    if(data != BMP180_WHO_AM_I_VAL)
    {
        return 2;
    }
    dump_calibration_data(device);
    return 0;
}
