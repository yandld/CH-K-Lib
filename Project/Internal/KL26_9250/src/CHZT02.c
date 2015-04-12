#include "gpio.h"
#include "common.h"
#include "CHZT02.h"
#include "CH9250.h"
#include "stdio.h"
#include "systick.h"

extern float gyroBias[3], accelBias[3]; 	/**from CH9250.c*/
extern float magCalibration[3], magbias[3]; /**from CH9250.c*/
extern uint8_t Ascale;      				/**from CH9250.c*/
extern uint8_t Gscale;  					/**from CH9250.c*/						
extern uint8_t Mscale;  					/**from CH9250.c*/		
extern uint8_t Mmode;  						/**from CH9250.c*/
extern float aRes, gRes, mRes;				/**from CH9250.c*/
int8_t CHZT02_Init(void) {
	/**Switch mosfet startup*/
	GPIO_QuickInit(CHZT02_SensorEN_Group, CHZT02_SensorEN_Pin, CHZT02_SensorEN_OutputMode);
	GPIO_WriteBit(CHZT02_SensorEN_Group, CHZT02_SensorEN_Pin, CHZT02_SensorEnable);
	/**I2C Init*/
	I2C_QuickInit(CHZT02_I2C_MAP,180000);
	/**MPU9250 Init*/
	uint8_t mpu9250_ID = 0;
	funsionDataInit();						/**global value overtake*/
	ch9250GetId(&mpu9250_ID);
	/**optional*/	printf("CHZT02_Init-------sensor MPU9250 id is 0x%x\n\r",mpu9250_ID);
	ch9250Reset();
	ch9250Calibrate(gyroBias, accelBias);
	/**optional*/	printf("x gyro bias = %f\n\r", gyroBias[0]);
	/**optional*/	printf("y gyro bias = %f\n\r", gyroBias[1]);
	/**optional*/	printf("z gyro bias = %f\n\r", gyroBias[2]);
	/**optional*/	printf("x accel bias = %f\n\r",accelBias[0]);
	/**optional*/	printf("y accel bias = %f\n\r",accelBias[1]);
	/**optional*/	printf("z accel bias = %f\n\r",accelBias[2]);
	DelayMs(200);
	ch9250Init();
	/**optional*/	printf("MPU9250 initialized for active data mode....\n\r");
	ch8963Init(magCalibration);
	/**optional*/	printf("AK8963 initialized for active data mode....\n\r"); // Initialize device for active mode read of magnetometer
	/**optional*/	printf("Accelerometer full-scale range = %f  g\n\r", 2.0f*(float)(1<<Ascale));
	/**optional*/	printf("Gyroscope full-scale range = %f  deg/s\n\r", 250.0f*(float)(1<<Gscale));
	/**optional*/	ch9250RegisterValueCheck();
	/**optional*/	if(Mscale == 0) printf("Magnetometer resolution = 14  bits\n\r");
	/**optional*/	if(Mscale == 1) printf("Magnetometer resolution = 16  bits\n\r");
	/**optional*/	if(Mmode == 2)  printf("Magnetometer ODR = 8 Hz\n\r");
	/**optional*/	if(Mmode == 6)  printf("Magnetometer ODR = 100 Hz\n\r");
	DelayMs(200);					
	ch9250GetAres(); // Get accelerometer sensitivity
    ch9250GetGres(); // Get gyro sensitivity
    ch9250GetMres(); // Get magnetometer sensitivity					
 	/**optional*/	printf("Accelerometer sensitivity is %f LSB/g \n\r", 1.0f/aRes);
 	/**optional*/	printf("Gyroscope sensitivity is %f LSB/deg/s \n\r", 1.0f/gRes);
 	/**optional*/	printf("Magnetometer sensitivity is %f LSB/G \n\r", 1.0f/mRes);						
	magbias[0] = +470.;  // User environmental x-axis correction in milliGauss, should be automatically calculated
    magbias[1] = +120.;  // User environmental x-axis correction in milliGauss
    magbias[2] = +125.;  // User environmental x-axis correction in milliGauss					
	return 0;
}

