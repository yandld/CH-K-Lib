#include "CH9250.h"
#include "stdio.h"
int8_t CH9250_Init(void){
	uint8_t id;
	
	if(!I2C_ReadSingleRegister(0, MPU9250_ADDR, MPU09250_WHO_AM_I, &id))
        {
			printf("MPU9250ID is 0x%x\n\r",id);
			I2C_ReadSingleRegister(0, MPU9250_ADDR, MPU09250_EXT_SENS_DATA_00, &id);
			printf("AK8975 ID is 0x%x\n\r",id); 
			if(id == MPU09250_ID)
            {   
                /* init sequence */
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_1,0x80);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_1,0x01); 
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_PWR_MGMT_2,0x00);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_CONFIG,0x07);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_GYRO_CONFIG,0x00);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_CONFIG,0x00);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_ACCEL_CONFIG2,0x00);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_INT_PIN_CFG,0x30);
				I2C_WriteSingleRegister(0,MPU9250_ADDR,MPU09250_I2C_MST_CTRL,0x40);			
                return 0;     
            }
			else
				printf("error mpu9250 ID,please check ic id\n\r");
        }
	return 0;
}
