#include "CH9250.h"
#include "stdio.h"
#include "stdint.h"
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
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_1,0x00);
	DelayMs(100);
	/** get stable time source*/
	/**Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001*/
	/**Auto selects the best available clock source – PLL if ready, else use the Internal oscillator*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_1,0x01);
	/** Configure Gyro and Accelerometer*/
	/**Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;*/ 
	/**DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both*/
	/**Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate	*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_CONFIG,0x03);
	/**Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)*/
	/**Use a 200 Hz rate; the same rate set in CONFIG above*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_SMPLRT_DIV,0x04);
	/**Set gyroscope full scale range */
	/**Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3 */
	uint8_t r;
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_CONFIG,&r);/**get status of register*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_CONFIG,r &~ 0xE0U);/** Clear self-test bits [7:5]*/
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_CONFIG,r &~ 0x18U);/**Clear AFS bits [4:3]*/	
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_CONFIG,r | (Gscale << 3U));/**Set full scale range for the accelerometer */	
	/** Set accelerometer sample rate configuration */
	/** It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for */
	/** accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz */
	I2C_ReadSingleRegister(0,MPU9250_GYRO_CONFIG, MPU9250_ACCEL_CONFIG2,&r);
	I2C_WriteSingleRegister(0,MPU9250_GYRO_CONFIG, MPU9250_ACCEL_CONFIG2, r & ~0x0FU); /** Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])*/  
	I2C_WriteSingleRegister(0,MPU9250_GYRO_CONFIG, MPU9250_ACCEL_CONFIG2, r | 0x03U); /** Set accelerometer rate to 1 kHz and bandwidth to 41 Hz*/	
	// The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
	// but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting
	// Configure Interrupts and Bypass Enable
	// Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips 
	// can join the I2C bus and all can be controlled by the Kinetis as master
	I2C_WriteSingleRegister(0, MPU9250_GYRO_CONFIG, MPU9250_INT_PIN_CFG, 0x22U);    
	I2C_WriteSingleRegister(0, MPU9250_GYRO_CONFIG, MPU9250_INT_ENABLE, 0x01U);  // Enable data ready (bit 0) interrupt
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
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_1,0x80);
	return 0;
}

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
int8_t ch9250Calibrate(float * dest1, float * dest2) {
	uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
	uint16_t ii, packet_count, fifo_count;
	int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
	// reset device, reset all registers, clear gyro and accelerometer bias registers
	ch9250Reset();
	DelayMs(10);
	// get stable time source
	// Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_1,0x01);
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_2,0x00);
	DelayMs(20);
	// Configure device for bias calculation
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_INT_ENABLE, 0x00);   // Disable all interrupts
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_FIFO_EN, 0x00);      // Disable FIFO
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_PWR_MGMT_1, 0x00);   // Turn on internal clock source
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_I2C_MST_CTRL, 0x00); // Disable I2C master
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_USER_CTRL, 0x0C);    // Reset FIFO and DMP
	DelayMs(15);
	// Configure MPU9250 gyro and accelerometer for bias calculation
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_CONFIG, 0x01);       // Set low-pass filter to 188 Hz
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_SMPLRT_DIV, 0x00);   // Set sample rate to 1 kHz
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

	uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
	uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
	// Configure FIFO to capture accelerometer and gyro data for bias calculation
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_USER_CTRL, 0x40);   // Enable FIFO  
	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
	DelayMs(4); // accumulate 40 samples in 80 milliseconds = 480 bytes
	
	// At end of sample accumulation, turn off FIFO sensor read
	I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU9250_FIFO_EN, 0x00);        	// Disable gyro and accelerometer sensors for FIFO
	I2C_BurstRead(0, MPU9250_ADDR, MPU9250_FIFO_COUNTH, 1, &data[0], 2);	// read FIFO sample count
	fifo_count = ((uint16_t)data[0] << 8) | data[1];
	packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging
	
	for (ii = 0; ii < packet_count; ii++) {
		int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
		I2C_BurstRead(0, MPU9250_ADDR, MPU9250_FIFO_R_W, 1, &data[0], 12); // read data for averaging
		accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
		accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
		accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;    
		gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
		gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
		gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;    
		accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
		accel_bias[1] += (int32_t) accel_temp[1];
		accel_bias[2] += (int32_t) accel_temp[2];
		gyro_bias[0]  += (int32_t) gyro_temp[0];
		gyro_bias[1]  += (int32_t) gyro_temp[1];
		gyro_bias[2]  += (int32_t) gyro_temp[2];
	}
	accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;
    if(accel_bias[2] > 0L) {
		accel_bias[2] -= (int32_t) accelsensitivity;// Remove gravity from the z-axis accelerometer bias calculation
	} else {
		accel_bias[2] += (int32_t) accelsensitivity;
	}
	//Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
	data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
	data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
	data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
	data[3] = (-gyro_bias[1]/4)       & 0xFF;
	data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
	data[5] = (-gyro_bias[2]/4)       & 0xFF;
/// Push gyro biases to hardware registers
//	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_XG_OFFSET_H, data[0]);
//	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_XG_OFFSET_L, data[1]);
//	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_YG_OFFSET_H, data[2]);
//	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_YG_OFFSET_L, data[3]);
//	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_ZG_OFFSET_H, data[4]);
//	I2C_WriteSingleRegister(0,MPU9250_ADDR, MPU9250_ZG_OFFSET_L, data[5]);
	dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
	dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
	dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;
	// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
	// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
	// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
	// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
	// the accelerometer biases calculated above must be divided by 8.	
	int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
	I2C_BurstRead(0, MPU9250_ADDR, MPU9250_XA_OFFSET_H, 1, &data[0], 2); // Read factory accelerometer trim values
	accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
	I2C_BurstRead(0, MPU9250_ADDR, MPU9250_YA_OFFSET_H, 1, &data[0], 2);
	accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
	I2C_BurstRead(0, MPU9250_ADDR, MPU9250_ZA_OFFSET_H, 1, &data[0], 2);
	accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];
	return 0;
}

int8_t ch9250RegisterValueCheck(void){
	uint8_t value = 0;
	printf("\f");
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SELF_TEST_X_GYRO,&value);		printf("MPU9250_SELF_TEST_X_GYRO	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SELF_TEST_Y_GYRO,&value);		printf("MPU9250_SELF_TEST_Y_GYRO	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SELF_TEST_Z_GYRO,&value);		printf("MPU9250_SELF_TEST_Z_GYRO	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SELF_TEST_X_ACCEL,&value);		printf("MPU9250_SELF_TEST_X_ACCEL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SELF_TEST_Y_ACCEL,&value);		printf("MPU9250_SELF_TEST_Y_ACCEL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SELF_TEST_Z_ACCEL,&value);		printf("MPU9250_SELF_TEST_Z_ACCEL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_XG_OFFSET_H,&value);				printf("MPU9250_XG_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_XG_OFFSET_L,&value);				printf("MPU9250_XG_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_YG_OFFSET_H,&value);				printf("MPU9250_YG_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_YG_OFFSET_L,&value);				printf("MPU9250_YG_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ZG_OFFSET_H,&value);				printf("MPU9250_ZG_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ZG_OFFSET_L,&value);				printf("MPU9250_ZG_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SMPLRT_DIV,&value);				printf("MPU9250_SMPLRT_DIV		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_CONFIG,&value);					printf("MPU9250_CONFIG			0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_CONFIG,&value);				printf("MPU9250_GYRO_CONFIG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_CONFIG,&value);			printf("MPU9250_ACCEL_CONFIG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_CONFIG2,&value);			printf("MPU9250_ACCEL_CONFIG2 		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_LP_ACCEL_ODR,&value);			printf("MPU9250_LP_ACCEL_ODR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_WOM_THR,&value);					printf("MPU9250_WOM_THR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_FIFO_EN,&value);					printf("MPU9250_FIFO_EN		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_MST_CTRL,&value);			printf("MPU9250_I2C_MST_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV0_ADDR,&value);			printf("MPU09250_I2C_SLV0_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV0_REG,&value);			printf("MPU9250_I2C_SLV0_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV0_CTRL,&value);			printf("MPU9250_I2C_SLV0_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV1_ADDR,&value);			printf("MPU9250_I2C_SLV1_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV1_REG,&value);			printf("MPU9250_I2C_SLV1_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV1_CTRL,&value);			printf("MPU9250_I2C_SLV1_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV2_ADDR,&value);			printf("MPU9250_I2C_SLV2_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV2_REG,&value);			printf("MPU9250_I2C_SLV2_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV2_CTRL,&value);			printf("MPU9250_I2C_SLV2_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV3_ADDR,&value);			printf("MPU9250_I2C_SLV3_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV3_REG,&value);			printf("MPU9250_I2C_SLV3_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV3_CTRL,&value);			printf("MPU9250_I2C_SLV3_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV4_ADDR,&value);			printf("MPU9250_I2C_SLV4_ADDR		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV4_REG,&value);			printf("MPU9250_I2C_SLV4_REG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV4_DO,&value);				printf("MPU9250_I2C_SLV4_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV4_CTRL,&value);			printf("MPU9250_I2C_SLV4_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV4_DI,&value);				printf("MPU9250_I2C_SLV4_DI		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_MST_STATUS,&value);			printf("MPU9250_I2C_MST_STATUS		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_INT_PIN_CFG,&value);				printf("MPU9250_INT_PIN_CFG		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_INT_ENABLE,&value);				printf("MPU9250_INT_ENABLE		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_INT_STATUS,&value);				printf("MPU9250_INT_STATUS		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_XOUT_H,&value);			printf("MPU9250_ACCEL_XOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_XOUT_L,&value);			printf("MPU9250_ACCEL_XOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_YOUT_H,&value);			printf("MPU9250_ACCEL_YOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_YOUT_L,&value);			printf("MPU9250_ACCEL_YOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_ZOUT_H,&value);			printf("MPU9250_ACCEL_ZOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ACCEL_ZOUT_L,&value);			printf("MPU9250_ACCEL_ZOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_TEMP_OUT_H,&value);				printf("MPU9250_TEMP_OUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_TEMP_OUT_L,&value);				printf("MPU9250_TEMP_OUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_XOUT_H,&value);				printf("MPU9250_GYRO_XOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_XOUT_L,&value);				printf("MPU9250_GYRO_XOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_YOUT_H,&value);				printf("MPU9250_GYRO_YOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_YOUT_L,&value);				printf("MPU9250_GYRO_YOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_ZOUT_H,&value);				printf("MPU9250_GYRO_ZOUT_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_GYRO_ZOUT_L,&value);				printf("MPU9250_GYRO_ZOUT_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_00,&value);		printf("MPU9250_EXT_SENS_DATA_00	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_01,&value);		printf("MPU9250_EXT_SENS_DATA_01	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_02,&value);		printf("MPU9250_EXT_SENS_DATA_02	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_03,&value);		printf("MPU9250_EXT_SENS_DATA_03	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_04,&value);		printf("MPU9250_EXT_SENS_DATA_04	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_05,&value);		printf("MPU9250_EXT_SENS_DATA_05	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_06,&value);		printf("MPU9250_EXT_SENS_DATA_06	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_07,&value);		printf("MPU9250_EXT_SENS_DATA_07	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_08,&value);		printf("MPU9250_EXT_SENS_DATA_08	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_09,&value);		printf("MPU9250_EXT_SENS_DATA_09	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_10,&value);		printf("MPU9250_EXT_SENS_DATA_10	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_11,&value);		printf("MPU9250_EXT_SENS_DATA_11	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_12,&value);		printf("MPU9250_EXT_SENS_DATA_12	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_13,&value);		printf("MPU9250_EXT_SENS_DATA_13	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_14,&value);		printf("MPU9250_EXT_SENS_DATA_14	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_15,&value);		printf("MPU9250_EXT_SENS_DATA_15	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_16,&value);		printf("MPU9250_EXT_SENS_DATA_16	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_17,&value);		printf("MPU9250_EXT_SENS_DATA_17	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_18,&value);		printf("MPU9250_EXT_SENS_DATA_18	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_19,&value);		printf("MPU9250_EXT_SENS_DATA_19	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_20,&value);		printf("MPU9250_EXT_SENS_DATA_20	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_21,&value);		printf("MPU9250_EXT_SENS_DATA_21	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_22,&value);		printf("MPU9250_EXT_SENS_DATA_22	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_EXT_SENS_DATA_23,&value);		printf("MPU9250_EXT_SENS_DATA_23	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV0_DO,&value);				printf("MPU9250_I2C_SLV0_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV1_DO,&value);				printf("MPU9250_I2C_SLV1_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV2_DO,&value);				printf("MPU9250_I2C_SLV2_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_SLV3_DO,&value);				printf("MPU9250_I2C_SLV3_DO		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_I2C_MST_DELAY_CTRL,&value);		printf("MPU9250_I2C_MST_DELAY_CTRL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_SIGNAL_PATH_RESET,&value);		printf("MPU9250_SIGNAL_PATH_RESET	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_MOT_DETECT_CTRL,&value);			printf("MPU9250_MOT_DETECT_CTRL	0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_USER_CTRL,&value);				printf("MPU9250_USER_CTRL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_1,&value);				printf("MPU9250_PWR_MGMT_1		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_PWR_MGMT_2,&value);				printf("MPU9250_PWR_MGMT_2		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_FIFO_COUNTH,&value);				printf("MPU9250_FIFO_COUNTH		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_FIFO_COUNTL,&value);				printf("MPU9250_FIFO_COUNTL		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_FIFO_R_W,&value);				printf("MPU9250_FIFO_R_W		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_WHO_AM_I,&value);				printf("MPU9250_WHO_AM_I		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_XA_OFFSET_H,&value);				printf("MPU9250_XA_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_XA_OFFSET_L,&value);				printf("MPU9250_XA_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_YA_OFFSET_H,&value);				printf("MPU9250_YA_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_YA_OFFSET_L,&value);				printf("MPU9250_YA_OFFSET_L		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ZA_OFFSET_H,&value);				printf("MPU9250_ZA_OFFSET_H		0x%x\n\r",value);
	I2C_ReadSingleRegister(0,MPU9250_ADDR,MPU9250_ZA_OFFSET_L,&value);				printf("MPU9250_ZA_OFFSET_L		0x%x\n\r",value);
	return 0;
}
     
