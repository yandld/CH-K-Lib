#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "pit.h"
#include "flash.h"
#include "dma.h"
#include "mpu9250.h"
#include "protocol.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "dmp.h"
#include "calibration.h"

#include "imu.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>


short gyro[3], accel[3], mag[3], sensors;


static void send_data_process(attitude_t *angle, int16_t *adata, int16_t *gdata, int16_t *mdata, int32_t pressure)
{
    int i;
    static uint8_t buf[64];
    uint32_t len;
    static payload_t payload;
    
    for(i=0;i<3;i++)
    {
        payload.acc[i] = adata[i];
        payload.gyo[i] = gdata[i];
        payload.mag[i] = mdata[i];
    }
    payload.P = (int16_t)(angle->P*100);
    payload.R = (int16_t)(angle->R*100);
    payload.Y = 1800 + (int16_t)(angle->Y*10);
    payload.pressure = pressure;
    /* set buffer */
    len = ano_encode(&payload, buf);
    
    if(UART_DMAGetRemain(HW_UART0) == 0)
    {
        UART_DMASend(HW_UART0, 0, buf, len);
    }
}


int main(void)
{
	uint32_t clock;
    uint32_t ret;
    static int16_t adata[3], gdata[3], mdata[3], cp_mdata[3];
    uint32_t fall;
    static float fadata[3], fgdata[3], fmdata[3];
    static attitude_t angle;
    struct mpu_config config;
    float pressure, dummy, temperature;
    
	DelayInit();   
    DelayMs(1);
    
    GPIO_Init(HW_GPIOC, 3, kGPIO_Mode_OPP);    
    UART_Init(UART0_RX_PA01_TX_PA02, 115200);    
    printf("CH IMU V1.2\r\n");

    /* MCU driver init */
    I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    DelayMs(10);
    
//    mpu9250_init(0);
//    
//    config.afs = AFS_8G;
//    config.gfs = GFS_500DPS;
//    config.mfs = MFS_14BITS;
//    config.aenable_self_test = false;
//    config.genable_self_test = false;
//    mpu9250_config(&config);



   // mpu9250_test();
    dmp_init();

    PIT_Init();
    
    
    static int time, last_time, load_val;
    static uint32_t fac_us;
    unsigned long sensor_timestamp;
    static unsigned char more;
    long quat[4];
    float dmp_angle[3];
    static float final_yaw;
    
    fac_us = GetClock(kBusClock);
    fac_us /= 1000000;

    while(1)
    {
        time = PIT_GetCnt(HW_PIT_CH1);
        PIT_SetTime(HW_PIT_CH1, 1000*1000);
        load_val = PIT_GetCnt(HW_PIT_CH1);
        time = load_val - time;
        time /= fac_us;

      //  mpu9250_read_accel_raw(adata);
    //    mpu9250_read_gyro_raw(gdata);
       // mpu9250_read_mag_raw(mdata);
        
//        mpu_get_compass_reg(mag, NULL);
        dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);	
        if (sensors & INV_WXYZ_QUAT )
        {
            quat2angle(quat, dmp_angle);
            adata[0] = accel[0];
            adata[1] = accel[1];
            adata[2] = accel[2];
            
            gdata[0] = gyro[0];
            gdata[1] = gyro[1];
            gdata[2] = gyro[2];            
            
            angle.P = dmp_angle[0];
            angle.R = dmp_angle[1];
            angle.Y = dmp_angle[2];
            
        //    printf("%f   %f   %f \r", dmp_angle[0], dmp_angle[1], dmp_angle[2]);

        }
        
    //    halfT = ((float)time)/1000/2000; 
        send_data_process(&angle, adata, gdata, cp_mdata, (int32_t)pressure);
     //   GPIO_ToggleBit(HW_GPIOC, 3);
    }
}


