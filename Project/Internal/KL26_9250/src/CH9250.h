#ifndef CH9250_H__
#define CH9250_H__
#include "common.h"
#include "i2c.h"
#define USE_MPU9250
/***********************Register Map**************************************/
#ifdef USE_MPU9250
#define	MPU9250_ADDR		0x68		/**9250硬件地址 0x68 0x69*/
#define MPU09250_ID			0x71		/**硬件识别ID*/

#define	MPU09250_SELF_TEST_X_GYRO		0x00		/**自检X轴角速度*/
#define	MPU09250_SELF_TEST_Y_GYRO		0x01		/**自检Y轴角速度*/
#define	MPU09250_SELF_TEST_Z_GYRO		0x02		/**自检Z轴角速度*/
#define	MPU09250_SELF_TEST_X_ACCEL		0x0D		/**自检X轴加速度*/
#define	MPU09250_SELF_TEST_Y_ACCEL		0x0E		/**自检Y轴加速度*/
#define	MPU09250_SELF_TEST_Z_ACCEL		0x0F		/**自检Z轴加速度*/
#define	MPU09250_XG_OFFSET_H			0x13		/**角速度直流偏置*/
#define	MPU09250_XG_OFFSET_L			0x14		/***/
#define	MPU09250_YG_OFFSET_H			0x15		/***/
#define	MPU09250_YG_OFFSET_L			0x16		/***/
#define	MPU09250_ZG_OFFSET_H			0x17		/***/
#define	MPU09250_ZG_OFFSET_L			0x18		/***/
#define	MPU09250_SMPLRT_DIV				0x19		/**采样率分频器*/
#define	MPU09250_CONFIG					0x1A		/***/
#define	MPU09250_GYRO_CONFIG			0x1B		/***/
#define	MPU09250_ACCEL_CONFIG			0x1C		/***/
#define	MPU09250_ACCEL_CONFIG2 			0x1D		/***/
#define	MPU09250_LP_ACCEL_ODR			0x1E		/***/
#define	MPU09250_WOM_THR				0x1F		/***/
#define	MPU09250_FIFO_EN				0x23		/***/
#define	MPU09250_I2C_MST_CTRL			0x24		/***/
#define	MPU09250_I2C_SLV0_ADDR			0x25		/***/
#define	MPU09250_I2C_SLV0_REG			0x26		/***/
#define	MPU09250_I2C_SLV0_CTRL			0x27		/***/
#define	MPU09250_I2C_SLV1_ADDR			0x28		/***/
#define	MPU09250_I2C_SLV1_REG			0x29		/***/
#define	MPU09250_I2C_SLV1_CTRL			0x2A		/***/
#define	MPU09250_I2C_SLV2_ADDR			0x2B		/***/
#define	MPU09250_I2C_SLV2_REG			0x2C		/***/
#define	MPU09250_I2C_SLV2_CTRL			0x2D		/***/
#define	MPU09250_I2C_SLV3_ADDR			0x2E		/***/
#define	MPU09250_I2C_SLV3_REG			0x2F		/***/
#define	MPU09250_I2C_SLV3_CTRL			0x30		/***/
#define	MPU09250_I2C_SLV4_ADDR			0x31		/***/
#define	MPU09250_I2C_SLV4_REG			0x32		/***/
#define	MPU09250_I2C_SLV4_DO			0x33		/***/
#define	MPU09250_I2C_SLV4_CTRL			0x34		/***/
#define	MPU09250_I2C_SLV4_DI			0x35		/***/
#define	MPU09250_I2C_MST_STATUS			0x36		/***/
#define	MPU09250_INT_PIN_CFG			0x37		/***/
#define	MPU09250_INT_ENABLE				0x38		/***/
#define	MPU09250_INT_STATUS				0x3A		/***/
#define	MPU09250_ACCEL_XOUT_H			0x3B		/***/
#define	MPU09250_ACCEL_XOUT_L			0x3C		/***/
#define	MPU09250_ACCEL_YOUT_H			0x3D		/***/
#define	MPU09250_ACCEL_YOUT_L			0x3E		/***/
#define	MPU09250_ACCEL_ZOUT_H			0x3F		/***/
#define	MPU09250_ACCEL_ZOUT_L			0x40		/***/
#define	MPU09250_TEMP_OUT_H				0x41		/***/
#define	MPU09250_TEMP_OUT_L				0x42		/***/
#define	MPU09250_GYRO_XOUT_H			0x43		/***/
#define	MPU09250_GYRO_XOUT_L			0x44		/***/
#define	MPU09250_GYRO_YOUT_H			0x45		/***/
#define	MPU09250_GYRO_YOUT_L			0x46		/***/
#define	MPU09250_GYRO_ZOUT_H			0x47		/***/
#define	MPU09250_GYRO_ZOUT_L			0x48		/***/
#define	MPU09250_EXT_SENS_DATA_00		0x49		/***/
#define	MPU09250_EXT_SENS_DATA_01		0x4A		/***/
#define	MPU09250_EXT_SENS_DATA_02		0x4B		/***/
#define	MPU09250_EXT_SENS_DATA_03		0x4C		/***/
#define	MPU09250_EXT_SENS_DATA_04		0x4D		/***/
#define	MPU09250_EXT_SENS_DATA_05		0x4E		/***/
#define	MPU09250_EXT_SENS_DATA_06		0x4F		/***/
#define	MPU09250_EXT_SENS_DATA_07		0x50		/***/
#define	MPU09250_EXT_SENS_DATA_08		0x51		/***/
#define	MPU09250_EXT_SENS_DATA_09		0x52		/***/
#define	MPU09250_EXT_SENS_DATA_10		0x53		/***/
#define	MPU09250_EXT_SENS_DATA_11		0x54		/***/
#define	MPU09250_EXT_SENS_DATA_12		0x55		/***/
#define	MPU09250_EXT_SENS_DATA_13		0x56		/***/
#define	MPU09250_EXT_SENS_DATA_14		0x57		/***/
#define	MPU09250_EXT_SENS_DATA_15		0x58		/***/
#define	MPU09250_EXT_SENS_DATA_16		0x59		/***/
#define	MPU09250_EXT_SENS_DATA_17		0x5A		/***/
#define	MPU09250_EXT_SENS_DATA_18		0x5B		/***/
#define	MPU09250_EXT_SENS_DATA_19		0x5C		/***/
#define	MPU09250_EXT_SENS_DATA_20		0x5D		/***/
#define	MPU09250_EXT_SENS_DATA_21		0x5E		/***/
#define	MPU09250_EXT_SENS_DATA_22		0x5F		/***/
#define	MPU09250_EXT_SENS_DATA_23		0x60		/***/
#define	MPU09250_I2C_SLV0_DO			0x63		/***/
#define	MPU09250_I2C_SLV1_DO			0x64		/***/
#define	MPU09250_I2C_SLV2_DO			0x65		/***/
#define	MPU09250_I2C_SLV3_DO			0x66		/***/
#define	MPU09250_I2C_MST_DELAY_CTRL		0x67		/***/
#define	MPU09250_SIGNAL_PATH_RESET		0x68		/***/
#define	MPU09250_MOT_DETECT_CTRL		0x69		/***/
#define	MPU09250_USER_CTRL				0x6A		/***/
#define	MPU09250_PWR_MGMT_1				0x6B		/***/
#define	MPU09250_PWR_MGMT_2				0x6C		/***/
#define	MPU09250_FIFO_COUNTH			0x72		/***/
#define	MPU09250_FIFO_COUNTL			0x73		/***/
#define	MPU09250_FIFO_R_W				0x74		/***/
#define	MPU09250_WHO_AM_I				0x75		/***/
#define	MPU09250_XA_OFFSET_H			0x77		/***/
#define	MPU09250_XA_OFFSET_L			0x78		/***/
#define	MPU09250_YA_OFFSET_H			0x7A		/***/
#define	MPU09250_YA_OFFSET_L			0x7B		/***/
#define	MPU09250_A_OFFSET_H				0x7D		/***/
#define	MPU09250_ZA_OFFSET_L			0x7E		/***/

#endif 
/***********************Register Map End**************************************/



int8_t CH9250_Init(void);
#else
#endif
