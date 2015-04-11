#include "CH9250.h"
#include "stdio.h"

/**Set initial input parameters*/
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

enum Mscale {
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS      // 0.15 mG per LSB
};

uint8_t Ascale = AFS_2G;      // AFS_2G, AFS_4G, AFS_8G, AFS_16G
uint8_t Gscale = GFS_250DPS;  // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
uint8_t Mscale = MFS_16BITS;  // MFS_14BITS or MFS_16BITS, 14-bit or 16-bit magnetometer resolution
uint8_t Mmode  = 0x06;        // Either 8 Hz 0x02) or 100 Hz (0x06) magnetometer data ODR  
float aRes, gRes, mRes;       // scale resolutions per LSB for the sensors

/*************************************************************************************************/


int8_t ch9250Init(void){
	/** Initialize MPU9250 device*/
	/** wake up device*/	
	/** Clear sleep mode bit (6), enable all sensors*/
	/**Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt*/  	
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_1,0x00);
	DelayMs(100);
	/** get stable time source*/
	/**Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001*/
	/**Auto selects the best available clock source – PLL if ready, else use the Internal oscillator*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_1,0x01);
	/** Configure Gyro and Accelerometer*/
	/**Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;*/ 
	/**DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both*/
	/**Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate	*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_CONFIG,0x03);
	/**Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)*/
	/**Use a 200 Hz rate; the same rate set in CONFIG above*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_SMPLRT_DIV,0x04);
	/**Set gyroscope full scale range */
	/**Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3 */
	uint8_t r;
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_CONFIG,&r);/**get status of register*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_CONFIG,r &~ 0xE0U);/** Clear self-test bits [7:5]*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_CONFIG,r &~ 0x18U);/**Clear AFS bits [4:3]*/	
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_CONFIG,r | (Gscale << 3U));/**Set full scale range for the accelerometer */	
	/** Set accelerometer sample rate configuration */
	/** It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for */
	/** accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz */
	I2C_ReadSingleRegister(0,MPU09250_GYRO_CONFIG, MPU09250_ACCEL_CONFIG2,&r);
	I2C_WriteSingleRegister(0,MPU09250_GYRO_CONFIG, MPU09250_ACCEL_CONFIG2, r & ~0x0FU); /** Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])*/  
	I2C_WriteSingleRegister(0,MPU09250_GYRO_CONFIG, MPU09250_ACCEL_CONFIG2, r | 0x03U); /** Set accelerometer rate to 1 kHz and bandwidth to 41 Hz*/	
	// The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
	// but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting
	// Configure Interrupts and Bypass Enable
	// Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips 
	// can join the I2C bus and all can be controlled by the Kinetis as master
	I2C_WriteSingleRegister(0, MPU09250_GYRO_CONFIG, MPU09250_INT_PIN_CFG, 0x22U);    
	I2C_WriteSingleRegister(0, MPU09250_GYRO_CONFIG, MPU09250_INT_ENABLE, 0x01U);  // Enable data ready (bit 0) interrupt
	return 0;
}

int8_t ch8963GetValue(float * destination){
	// First extract the factory calibration for each magnetometer axis
	uint8_t rawData[3] = {0};  // x/y/z gyro calibration data stored here
	I2C_WriteSingleRegister(0,AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
	DelayMs(10);
	I2C_WriteSingleRegister(0,AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
	DelayMs(10);
	I2C_BurstRead(0, AK8963_ADDRESS, AK8963_ASAX, 1, rawData, 3); // Read the x-, y-, and z-axis calibration values
	destination[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;   // Return x-axis sensitivity adjustment values, etc.
	destination[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;  
	destination[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
	I2C_WriteSingleRegister(0,AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
	DelayMs(10);	
	// Configure the magnetometer for continuous read and highest resolution
	// set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
	// and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
	I2C_WriteSingleRegister(0,AK8963_ADDRESS, AK8963_CNTL, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
	DelayMs(10);
	return 0;
}
int8_t ch9250Reset(void){
	/**Write a one to bit 7 reset bit; toggle reset device*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_1,0x80);
}
int8_t ch9250RegisterValueCheck(void){
	uint8_t value = 0;
	printf("\f");
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SELF_TEST_X_GYRO,&value);		printf("MPU09250_SELF_TEST_X_GYRO	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SELF_TEST_Y_GYRO,&value);		printf("MPU09250_SELF_TEST_Y_GYRO	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SELF_TEST_Z_GYRO,&value);		printf("MPU09250_SELF_TEST_Z_GYRO	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SELF_TEST_X_ACCEL,&value);		printf("MPU09250_SELF_TEST_X_ACCEL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SELF_TEST_Y_ACCEL,&value);		printf("MPU09250_SELF_TEST_Y_ACCEL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SELF_TEST_Z_ACCEL,&value);		printf("MPU09250_SELF_TEST_Z_ACCEL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_XG_OFFSET_H,&value);				printf("MPU09250_XG_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_XG_OFFSET_L,&value);				printf("MPU09250_XG_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_YG_OFFSET_H,&value);				printf("MPU09250_YG_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_YG_OFFSET_L,&value);				printf("MPU09250_YG_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ZG_OFFSET_H,&value);				printf("MPU09250_ZG_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ZG_OFFSET_L,&value);				printf("MPU09250_ZG_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SMPLRT_DIV,&value);				printf("MPU09250_SMPLRT_DIV		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_CONFIG,&value);					printf("MPU09250_CONFIG			0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_CONFIG,&value);				printf("MPU09250_GYRO_CONFIG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_CONFIG,&value);			printf("MPU09250_ACCEL_CONFIG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_CONFIG2,&value);			printf("MPU09250_ACCEL_CONFIG2 		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_LP_ACCEL_ODR,&value);			printf("MPU09250_LP_ACCEL_ODR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_WOM_THR,&value);					printf("MPU09250_WOM_THR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_FIFO_EN,&value);					printf("MPU09250_FIFO_EN		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_MST_CTRL,&value);			printf("MPU09250_I2C_MST_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV0_ADDR,&value);			printf("MPU09250_I2C_SLV0_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV0_REG,&value);			printf("MPU09250_I2C_SLV0_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV0_CTRL,&value);			printf("MPU09250_I2C_SLV0_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV1_ADDR,&value);			printf("MPU09250_I2C_SLV1_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV1_REG,&value);			printf("MPU09250_I2C_SLV1_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV1_CTRL,&value);			printf("MPU09250_I2C_SLV1_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV2_ADDR,&value);			printf("MPU09250_I2C_SLV2_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV2_REG,&value);			printf("MPU09250_I2C_SLV2_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV2_CTRL,&value);			printf("MPU09250_I2C_SLV2_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV3_ADDR,&value);			printf("MPU09250_I2C_SLV3_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV3_REG,&value);			printf("MPU09250_I2C_SLV3_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV3_CTRL,&value);			printf("MPU09250_I2C_SLV3_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV4_ADDR,&value);			printf("MPU09250_I2C_SLV4_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV4_REG,&value);			printf("MPU09250_I2C_SLV4_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV4_DO,&value);				printf("MPU09250_I2C_SLV4_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV4_CTRL,&value);			printf("MPU09250_I2C_SLV4_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV4_DI,&value);				printf("MPU09250_I2C_SLV4_DI		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_MST_STATUS,&value);			printf("MPU09250_I2C_MST_STATUS		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_INT_PIN_CFG,&value);				printf("MPU09250_INT_PIN_CFG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_INT_ENABLE,&value);				printf("MPU09250_INT_ENABLE		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_INT_STATUS,&value);				printf("MPU09250_INT_STATUS		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_XOUT_H,&value);			printf("MPU09250_ACCEL_XOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_XOUT_L,&value);			printf("MPU09250_ACCEL_XOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_YOUT_H,&value);			printf("MPU09250_ACCEL_YOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_YOUT_L,&value);			printf("MPU09250_ACCEL_YOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_ZOUT_H,&value);			printf("MPU09250_ACCEL_ZOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_ZOUT_L,&value);			printf("MPU09250_ACCEL_ZOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_TEMP_OUT_H,&value);				printf("MPU09250_TEMP_OUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_TEMP_OUT_L,&value);				printf("MPU09250_TEMP_OUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_XOUT_H,&value);				printf("MPU09250_GYRO_XOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_XOUT_L,&value);				printf("MPU09250_GYRO_XOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_YOUT_H,&value);				printf("MPU09250_GYRO_YOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_YOUT_L,&value);				printf("MPU09250_GYRO_YOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_ZOUT_H,&value);				printf("MPU09250_GYRO_ZOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_ZOUT_L,&value);				printf("MPU09250_GYRO_ZOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_00,&value);		printf("MPU09250_EXT_SENS_DATA_00	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_01,&value);		printf("MPU09250_EXT_SENS_DATA_01	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_02,&value);		printf("MPU09250_EXT_SENS_DATA_02	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_03,&value);		printf("MPU09250_EXT_SENS_DATA_03	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_04,&value);		printf("MPU09250_EXT_SENS_DATA_04	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_05,&value);		printf("MPU09250_EXT_SENS_DATA_05	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_06,&value);		printf("MPU09250_EXT_SENS_DATA_06	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_07,&value);		printf("MPU09250_EXT_SENS_DATA_07	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_08,&value);		printf("MPU09250_EXT_SENS_DATA_08	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_09,&value);		printf("MPU09250_EXT_SENS_DATA_09	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_10,&value);		printf("MPU09250_EXT_SENS_DATA_10	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_11,&value);		printf("MPU09250_EXT_SENS_DATA_11	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_12,&value);		printf("MPU09250_EXT_SENS_DATA_12	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_13,&value);		printf("MPU09250_EXT_SENS_DATA_13	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_14,&value);		printf("MPU09250_EXT_SENS_DATA_14	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_15,&value);		printf("MPU09250_EXT_SENS_DATA_15	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_16,&value);		printf("MPU09250_EXT_SENS_DATA_16	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_17,&value);		printf("MPU09250_EXT_SENS_DATA_17	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_18,&value);		printf("MPU09250_EXT_SENS_DATA_18	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_19,&value);		printf("MPU09250_EXT_SENS_DATA_19	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_20,&value);		printf("MPU09250_EXT_SENS_DATA_20	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_21,&value);		printf("MPU09250_EXT_SENS_DATA_21	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_22,&value);		printf("MPU09250_EXT_SENS_DATA_22	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_EXT_SENS_DATA_23,&value);		printf("MPU09250_EXT_SENS_DATA_23	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV0_DO,&value);				printf("MPU09250_I2C_SLV0_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV1_DO,&value);				printf("MPU09250_I2C_SLV1_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV2_DO,&value);				printf("MPU09250_I2C_SLV2_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_SLV3_DO,&value);				printf("MPU09250_I2C_SLV3_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_MST_DELAY_CTRL,&value);		printf("MPU09250_I2C_MST_DELAY_CTRL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_SIGNAL_PATH_RESET,&value);		printf("MPU09250_SIGNAL_PATH_RESET	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_MOT_DETECT_CTRL,&value);			printf("MPU09250_MOT_DETECT_CTRL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_USER_CTRL,&value);				printf("MPU09250_USER_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_1,&value);				printf("MPU09250_PWR_MGMT_1		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_2,&value);				printf("MPU09250_PWR_MGMT_2		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_FIFO_COUNTH,&value);				printf("MPU09250_FIFO_COUNTH		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_FIFO_COUNTL,&value);				printf("MPU09250_FIFO_COUNTL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_FIFO_R_W,&value);				printf("MPU09250_FIFO_R_W		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_WHO_AM_I,&value);				printf("MPU09250_WHO_AM_I		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_XA_OFFSET_H,&value);				printf("MPU09250_XA_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_XA_OFFSET_L,&value);				printf("MPU09250_XA_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_YA_OFFSET_H,&value);				printf("MPU09250_YA_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_YA_OFFSET_L,&value);				printf("MPU09250_YA_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_A_OFFSET_H,&value);				printf("MPU09250_A_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU09250_ZA_OFFSET_L,&value);				printf("MPU09250_ZA_OFFSET_L		0x%x\n\r",value);
	return 0;
}
            