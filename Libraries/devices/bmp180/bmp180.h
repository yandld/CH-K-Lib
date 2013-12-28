#ifndef __BMP180_H__
#define __BMP180_H__


#include <stdint.h>

typedef struct bmp180_device
{
    const char* name;
    uint32_t I2CxMAP;
    uint8_t device_addr;
    uint8_t i2c_instance;
    uint8_t (*read_reg)(struct bmp180_device * device, uint8_t reg_addr, uint8_t *data);
    uint8_t (*write_reg)(struct bmp180_device * device, uint8_t reg_addr, uint8_t data);
    uint8_t (*read_temperature)(struct bmp180_device * device, int32_t* temperature);
    uint8_t (*read_pressure)(struct bmp180_device * device, int32_t* pressure);
    uint8_t (*start_conversion)(struct bmp180_device * device, uint8_t mode);
    uint8_t (*ctrl)(struct bmp180_device * device, uint8_t cmd);
    uint8_t (*probe)(struct bmp180_device * device);
    void * bmp_data;
}bmp180_device;

/* BMP180 defines */
#define BMP180_ADDR                     0x77 // BMP180 address
/* BMP180 registers */
#define BMP180_PROM_START_ADDR          0xAA // E2PROM calibration data start register
#define BMP180_PROM_DATA_LEN            22   // E2PROM length
#define BMP180_CHIP_ID_REG              0xD0 // Chip ID
#define BMP180_VERSION_REG              0xD1 // Version
#define BMP180_CTRL_MEAS_REG            0xF4 // Measurements control (OSS[7.6], SCO[5], CTL[4.0]
#define BMP180_ADC_OUT_MSB_REG          0xF6 // ADC out MSB  [7:0]
#define BMP180_ADC_OUT_LSB_REG          0xF7 // ADC out LSB  [7:0]
#define BMP180_ADC_OUT_XLSB_REG         0xF8 // ADC out XLSB [7:3]
#define BMP180_SOFT_RESET_REG           0xE0 // Soft reset control
/* BMP180 control values */
#define BMP180_T_MEASURE                0x2E // temperature measurement
#define BMP180_P0_MEASURE               0x34 // pressure measurement (OSS=0, 4.5ms)
#define BMP180_P1_MEASURE               0x74 // pressure measurement (OSS=1, 7.5ms)
#define BMP180_P2_MEASURE               0xB4 // pressure measurement (OSS=2, 13.5ms)
#define BMP180_P3_MEASURE               0xF4 // pressure measurement (OSS=3, 25.5ms)
/* BMP180 Pressure calculation constants */
#define BMP180_PARAM_MG                 3038
#define BMP180_PARAM_MH                -7357
#define BMP180_PARAM_MI                 3791

#define BMP180_REG_AC1 0xaa
#define BMP180_REG_AC2 0xac
#define BMP180_REG_AC3 0xae
#define BMP180_REG_AC4 0xb0
#define BMP180_REG_AC5 0xb2
#define BMP180_REG_AC6 0xb4
#define BMP180_REG_B1 0xb6
#define BMP180_REG_B2 0xb8
#define BMP180_REG_MB 0xba
#define BMP180_REG_MC 0xbc
#define BMP180_REG_MD 0xbe


//!< API functions
uint8_t bmp180_init(bmp180_device* device, uint32_t I2CxMap, const char * name, uint32_t baudrate);

#endif

