/**
  ******************************************************************************
  * @file    bmp180.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
  
#include <string.h>
#include <math.h>
#include "bmp180.h"
#include "i2c.h"

/* BMP180 control values and cmd*/
#define BMP180_T_MEASURE                0x2E /* temperature measurement */
#define BMP180_P0_MEASURE               0x34 /* pressure measurement (OSS=0, 4.5ms) */
#define BMP180_P1_MEASURE               0x74 /* pressure measurement (OSS=1, 7.5ms) */
#define BMP180_P2_MEASURE               0xB4 /* pressure measurement (OSS=2, 13.5ms) */
#define BMP180_P3_MEASURE               0xF4 /* pressure measurement (OSS=3, 25.5ms) */

#define BMP180_DEBUG		1
#if ( BMP180_DEBUG == 1 )
#include <stdio.h>
#define BMP180_TRACE	printf
#else
#define BMP180_TRACE(...)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif
#ifndef SWAP16
#define  SWAP16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) 
#endif

/* BMP180 registers */
#define BMP180_PROM_START_ADDR          0xAA /* E2PROM calibration data start register */
#define BMP180_PROM_DATA_LEN            22   /* E2PROM length */
#define BMP180_CHIP_ID_REG              0xD0 /* Chip ID */
#define BMP180_VERSION_REG              0xD1 /* Version */
#define BMP180_CTRL_MEAS_REG            0xF4 /* Measurements control (OSS[7.6], SCO[5], CTL[4.0] */
#define BMP180_ADC_OUT_MSB_REG          0xF6 /* ADC out MSB  [7:0] */
#define BMP180_ADC_OUT_LSB_REG          0xF7 /* ADC out LSB  [7:0] */
#define BMP180_ADC_OUT_XLSB_REG         0xF8 /* ADC out XLSB [7:3] */
#define BMP180_SOFT_RESET_REG           0xE0 /* Soft reset control */
/* BMP180 Pressure calculation constants */
#define BMP180_PARAM_MG                 3038
#define BMP180_PARAM_MH                -7357
#define BMP180_PARAM_MI                 3791
/* BMP180 Calibration coefficients */
#define BMP180_REG_AC1                  0xAA
#define BMP180_REG_AC2                  0xAC
#define BMP180_REG_AC3                  0xAE
#define BMP180_REG_AC4                  0xB0
#define BMP180_REG_AC5                  0xB2
#define BMP180_REG_AC6                  0xB4
#define BMP180_REG_B1                   0xB6
#define BMP180_REG_B2                   0xB8
#define BMP180_REG_MB                   0xBA
#define BMP180_REG_MC                   0xBC
#define BMP180_REG_MD                   0xBE

#define BMP085_ULTRALOWPOWER 0
#define BMP085_STANDARD      1
#define BMP085_HIGHRES       2
#define BMP085_ULTRAHIGHRES  3
#define BMP085_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3           0xAE  // R   Calibration data (16 bits)    
#define BMP085_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD            0xBE  // R   Calibration data (16 bits)

#define BMP085_CONTROL           0xF4 
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD          0x2E
#define BMP085_READPRESSURECMD            0x34

int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;
uint8_t oversampling;

__packed struct bmp180_cal
{
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
    int16_t B5; /* B5 is intermediate variable for pressure compensatation */
};


struct bmp_device 
{
    uint8_t     addr;
    uint32_t    instance;
    void        *user_data;
};

static struct bmp_device bmp_dev;
static const uint8_t bmp_addr[] = {0x77};
static struct bmp180_cal cal;

static int write8(uint8_t addr, uint8_t val)
{
    return I2C_WriteSingleRegister(bmp_dev.instance, bmp_dev.addr, addr, val);
}

static int16_t read16(uint8_t addr)
{
    int16_t data;
    I2C_BurstRead(bmp_dev.instance, bmp_dev.addr, addr, 1, (uint8_t*)&data, sizeof(data));
    return data;
}

static int read8(uint8_t addr, uint8_t *val)
{
    return I2C_ReadSingleRegister(bmp_dev.instance, bmp_dev.addr, addr, val);
}

/*!
 * @brief dump bmp180 calibration data from bmp180 internal eeporm to struct
 *
 * @param pointer of the bmp180 device struct
 */
static int dump_calibration_data(void)
{
    I2C_BurstRead(bmp_dev.instance, bmp_dev.addr, BMP180_PROM_START_ADDR, 1, (uint8_t*)&cal, BMP180_PROM_DATA_LEN);

    /* read calibration data */
    ac1 = read16(BMP085_CAL_AC1);
    ac2 = read16(BMP085_CAL_AC2);
    ac3 = read16(BMP085_CAL_AC3);
    ac4 = read16(BMP085_CAL_AC4);
    ac5 = read16(BMP085_CAL_AC5);
    ac6 = read16(BMP085_CAL_AC6);

    b1 = read16(BMP085_CAL_B1);
    b2 = read16(BMP085_CAL_B2);

    mb = read16(BMP085_CAL_MB);
    mc = read16(BMP085_CAL_MC);
    md = read16(BMP085_CAL_MD);
    
    cal.AC1 = SWAP16(cal.AC1);
    cal.AC2 = SWAP16(cal.AC2);
    cal.AC3 = SWAP16(cal.AC3);
    cal.AC4 = SWAP16(cal.AC4);
    cal.AC5 = SWAP16(cal.AC5);
    cal.AC6 = SWAP16(cal.AC6);
    cal.B1 = SWAP16(cal.B1);
    cal.B2 = SWAP16(cal.B2);
    cal.B5 = SWAP16(cal.B5);
    cal.MB = SWAP16(cal.MB);
    cal.MC = SWAP16(cal.MC);
    cal.MD = SWAP16(cal.MD);
    BMP180_TRACE("ac1:%d\r\n", ac1);
    BMP180_TRACE("ac2:%d\r\n", ac2);
    BMP180_TRACE("ac3:%d\r\n", ac3);
    BMP180_TRACE("ac4:%d\r\n", ac4); 
    BMP180_TRACE("ac5:%d\r\n", ac5);
    BMP180_TRACE("ac6:%d\r\n", ac6);
    BMP180_TRACE("b1:%d\r\n", b1);
    BMP180_TRACE("b2:%d\r\n", b2);
    BMP180_TRACE("mb:%d\r\n", mb);
    BMP180_TRACE("mc:%d\r\n", mc);
    BMP180_TRACE("md:%d\r\n", md);
    return 0;
}

static int32_t computeB5(int32_t UT)
{
    int32_t X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) >> 15;
    int32_t X2 = ((int32_t)mc << 11) / (X1+(int32_t)md);
    return X1 + X2;
}

static uint16_t readRawTemperature(void)
{
    uint16_t raw_temp;
    write8(BMP085_CONTROL, BMP085_READTEMPCMD);
    DelayMs(5);
    raw_temp = read16(BMP085_TEMPDATA);
    BMP180_TRACE("Raw temp:%d\r\n", raw_temp);
    return raw_temp;
}

static uint32_t readRawPressure(void)
{
  uint32_t raw;
  uint8_t data;
    
  write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

  if (oversampling == BMP085_ULTRALOWPOWER) 
    DelayMs(5);
  else if (oversampling == BMP085_STANDARD) 
    DelayMs(8);
  else if (oversampling == BMP085_HIGHRES) 
    DelayMs(14);
  else 
    DelayMs(26);

  raw = read16(BMP085_PRESSUREDATA);

  raw <<= 8;
  read8(BMP085_PRESSUREDATA+2, &data);
  raw |= data;
  raw >>= (8 - oversampling);

 /* this pull broke stuff, look at it later?
  if (oversampling==0) {
    raw <<= 8;
    raw |= read8(BMP085_PRESSUREDATA+2);
    raw >>= (8 - oversampling);
  }
 */
  return raw;
}



int32_t readPressure(void) {
  int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
  uint32_t B4, B7;

  UT = readRawTemperature();
  UP = readRawPressure();

#if BMP085_DEBUG == 1
  // use datasheet numbers!
  UT = 27898;
  UP = 23843;
  ac6 = 23153;
  ac5 = 32757;
  mc = -8711;
  md = 2868;
  b1 = 6190;
  b2 = 4;
  ac3 = -14383;
  ac2 = -72;
  ac1 = 408;
  ac4 = 32741;
  oversampling = 0;
#endif

  B5 = computeB5(UT);

#if BMP085_DEBUG == 1
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
  Serial.print("B5 = "); Serial.println(B5);
#endif

  // do pressure calcs
  B6 = B5 - 4000;
  X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
  X2 = ((int32_t)ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;

#if BMP085_DEBUG == 1
  Serial.print("B6 = "); Serial.println(B6);
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
  Serial.print("B3 = "); Serial.println(B3);
#endif

  X1 = ((int32_t)ac3 * B6) >> 13;
  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> oversampling );

#if BMP085_DEBUG == 1
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
  Serial.print("B4 = "); Serial.println(B4);
  Serial.print("B7 = "); Serial.println(B7);
#endif

  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;

#if BMP085_DEBUG == 1
  Serial.print("p = "); Serial.println(p);
  Serial.print("X1 = "); Serial.println(X1);
  Serial.print("X2 = "); Serial.println(X2);
#endif

  p = p + ((X1 + X2 + (int32_t)3791)>>4);
#if BMP085_DEBUG == 1
  Serial.print("p = "); Serial.println(p);
#endif
  return p;
}


int32_t readSealevelPressure(float altitude_meters) {
  float pressure = readPressure();
  return (int32_t)(pressure / pow(1.0-altitude_meters/44330, 5.255));
}


float readTemperature(void) {
  int32_t UT, B5;     // following ds convention
  float temp;

  UT = readRawTemperature();

#if BMP085_DEBUG == 1
  // use datasheet numbers!
  UT = 27898;
  ac6 = 23153;
  ac5 = 32757;
  mc = -8711;
  md = 2868;
#endif

  B5 = computeB5(UT);
  temp = (B5+8) >> 4;
  temp /= 10;
  
  return temp;
}


float readAltitude(float sealevelPressure) {
  float altitude;

  float pressure = readPressure();

  altitude = 44330 * (1.0 - pow(pressure /sealevelPressure,0.1903));

  return altitude;
}




int bmp180_init(uint32_t instance)
{
    int i;
    uint8_t id;
    
    bmp_dev.instance = instance;
    
    for(i = 0; i < ARRAY_SIZE(bmp_addr); i++)
    {
        if(!I2C_ReadSingleRegister(instance, bmp_addr[i], BMP180_CHIP_ID_REG, &id))
        {
            if(id == 0x55)
            {
                bmp_dev.addr = bmp_addr[i];
                BMP180_TRACE("BMP180 addr:0x%X\r\n", bmp_dev.addr);
                dump_calibration_data();
                return 0;     
            }
        }
    }
    return 1;
}


/*
 * @brief start bmp180 conversion
 *
 * @param pointer of the bmp180 device struct
 * @param conversion mode: 
 *        @arg BMP180_T_MEASURE: temperature conversion
 *        @arg BMP180_P0_MEASURE: pressure conversion, oversampling:0
 *        @arg BMP180_P1_MEASURE: pressure conversion, oversampling:1
 *        @arg BMP180_P2_MEASURE: pressure conversion, oversampling:2
 */
int bmp180_start_conversion(uint8_t cmd)
{
    return write8(BMP180_CTRL_MEAS_REG, cmd); 
}


/*!
 * @brief read raw temperature data
 *
 * @param pointer of the bmp180 device struct
 * @param data pointer
 */
static int read_raw_temperature(int32_t * data)
{
    uint8_t err;
    uint8_t buf[2];
    
    err = I2C_BurstRead(bmp_dev.instance, bmp_dev.addr, BMP180_ADC_OUT_MSB_REG, 1, buf, 2);
    
    *data = ((((int16_t)(buf[0])) << 8) + (int16_t)(buf[1]));
    return err;
}

/*!
 * @brief read raw perssure data
 *
 * @param pointer of the bmp180 device struct
 * @param data pointer
 * @param oversampling rate data pointer
 */
static int read_raw_pressure(int32_t * data, uint8_t *oss)
{
    uint8_t err;
    uint8_t buf[3];
    uint8_t reg1;
    
    /* read oss */
    read8(BMP180_CTRL_MEAS_REG, &reg1);
    *oss = reg1 >> 6;
    
    err = I2C_BurstRead(bmp_dev.instance, bmp_dev.addr, BMP180_ADC_OUT_MSB_REG, 1, buf, 3);

    *data = (buf[0] << 16) + (buf[1] << 8) + buf[2];
    *data >>= (8 - (*oss));
    return err;
}

/*!
 * @brief tell if bmp180 is in conversion
 *
 * @param pointer of the bmp180 device struct
 * @retval 1:busy 0:idle
 */
static int is_conversion_busy(void)
{
    uint8_t reg1;
    read8(BMP180_CTRL_MEAS_REG, &reg1);
    if(reg1 & (1 << 5)) /* busy */
    {
        return 1;
    }
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
static int bmp180_read_temperature(int32_t * temperature)
{
    int32_t raw_temperature;
    int32_t x1, x2;

    if(read_raw_temperature(&raw_temperature))
    {
        return 2;
    }

    BMP180_TRACE("raw_t = %d\n\r",raw_temperature);
    BMP180_TRACE("ac6 = %d\n\r",cal.AC6);
    BMP180_TRACE("ac5 = %d\n\r",cal.AC5);
    BMP180_TRACE("ac5/32768 = %f\n\r",((float)cal.AC5)/32768);
    BMP180_TRACE("raw_t-ac6 = %d\n\r",(raw_temperature - cal.AC6));

    x1 = (int32_t)((raw_temperature - cal.AC6) * (int32_t)cal.AC5) >> 15;
    x2 = (int32_t)(cal.MC <<11) / (x1 + cal.MD);
    cal.B5 = x1 + x2;
    *temperature = (cal.B5 + (int32_t)8) >> 4;

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
static int bmp180_read_pressure(int32_t * pressure)
{
    int32_t raw_pressure;
    uint8_t oss;
    /* read raw pressure */
    if(read_raw_pressure(&raw_pressure, &oss))
    {
        return 2;
    }
    /* temperature compensation */
    uint32_t b4, b7;
    int32_t x1, x2, x3, b3, b6;
    int32_t result;
    b6 = cal.B5 - 4000;
        
    x1 = (cal.B2 * (b6 * b6 >> 12)) >> 11;
    x2 = cal.AC2 * b6 >> 11;
    x3 = x1 + x2;

    b3 = ((((int32_t)cal.AC1 * 4 + x3) << oss) + 2) >> 2;
    x1 = cal.AC3 * b6 >> 13;
    x2 = (cal.B1 * (b6 * b6 >> 12)) >> 16;
    x3 = (x1 + x2 + 2) >> 2;
    b4 = (cal.AC4 * (uint32_t)(x3 + 32768)) >> 15;
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

int bmp180_pressure2altitude(int32_t pressure, int32_t *altitude)
{
    float fpressure;
    fpressure = (float)pressure;
	*altitude =44330 * (1.0 - pow(pressure /(float)101500,0.1903));
	return 0;
}


#define T_START          (1<<0)
#define T_COMPLETE       (1<<1)
#define P_START          (1<<2)
#define P_COMPLETE       (1<<3)
#define T_WAIT           (1<<4)
#define P_WAIT           (1<<5)

int bmp180_conversion_process(int32_t *pressure, int32_t *temperature)
{
    int ret;
    static int states = T_START;
    
    ret = 1;
    
    switch(states)
    {
        case T_START:
            bmp180_start_conversion(BMP180_T_MEASURE);
            states = T_WAIT;
            break;
        case T_WAIT:
            if(!is_conversion_busy())
            {
                states = T_COMPLETE;
            }
            break;
        case T_COMPLETE:
            bmp180_read_temperature(temperature);
            states = P_START;
            break;
        case P_START:
            bmp180_start_conversion(BMP180_P3_MEASURE);
            states = P_WAIT;
            break;
        case P_WAIT:
            if(!is_conversion_busy())
            {
                states = P_COMPLETE;
            }
            break;
        case P_COMPLETE:
            bmp180_read_pressure(pressure);
            states = T_START;
            ret = 0;
            break; 
    }
    return ret;
}

