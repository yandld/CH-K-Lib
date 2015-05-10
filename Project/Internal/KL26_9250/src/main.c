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


static imu_float_euler_angle_t imu_angle;
static imu_raw_data_t raw_data;
static struct calibration_data cal_data;
short gyro[3], accel[3], mag[3], sensors;
    
static void _print_cal_data(struct calibration_data * cal)
{
    printf("cal data:\r\n");
    printf("axo:%04d ayo:%04d azo:%04d\r\n", cal->axo, cal->ayo, cal->azo);
    printf("gxo:%04d gyo:%04d gzo:%04d\r\n", cal->gxo, cal->gyo, cal->gzo);
    printf("mxo:%04d myo:%04d mzo:%04d\r\n", cal->mxo, cal->myo, cal->mzo);
    printf("mxg:%04f myg:%04f mzg:%0fd\r\n", cal->mxg, cal->myg, cal->mzg);
    
}

void MagnetometerCalibration(struct calibration_data * cal)
{
    uint32_t i;
    int r;
    int16_t x,y,z;
    int16_t xmax, xmin, ymax, ymin, zmax, zmin;
    xmax = 0;
    xmin = 0xFFFF;
    ymax = 0;
    ymin = 0xFFFF;
    zmax = 0;
    zmin = 0xFFFF;
    printf("start calibration...\r\n");

    for(i=0;i<1000;i++)
    {
        r = mpu9250_read_mag_raw(&x, &y, &z);
        if(!r)
        {
            if(xmax < x) xmax = x;
            if(xmin > x) xmin = x;
            if(ymax < y) ymax = y;
            if(ymin > y) ymin = y;
            if(zmax < z) zmax = z;
            if(zmin > z) zmin = z; 
        }
        DelayMs(10);
        printf("time:%04d xmax:%04d xmin:%04d ymax:%04d ymin%04d zmax:%04d zmin:%04d\r", i,xmax,xmin,ymax,ymin,zmax,zmin);
    }
    cal->mxo = (xmax + xmin) / 2;
    cal->mxg=1;
    cal->myo = (ymax + ymin) / 2;
    cal->myg= (float)(ymax - ymin) / (float)(xmax -xmin);
    cal->mzo = (zmax + zmin) / 2;
    cal->mzg= (float)(zmax - zmin) / (float)(xmax -xmin);
    /* see if we get data correct */
    if((cal->myg < 0.8) || (cal->mzg < 0.8))
    {
        printf("cal failed, setting to default param\r\n");
        /* inject with default data */
        cal->mxo = 0;
        cal->myo = 0;
        cal->mzo = 0;
        cal->mxg = 1;
        cal->myg = 1;
        cal->mzg = 1;
        cal->magic = 0x00;
        return;
    }
    else
    {
        cal->magic = 0x5ACB;
    }
    printf("g X:%f Y:%f Z:%f\r\n", cal->mxg, cal->myg, cal->mzg);
    printf("o X:%d Y:%d Z:%d\r\n", cal->mxo, cal->myo, cal->mzo);
}


void mpu9250_test(void)
{
    mpu9250_init(0);
    struct mpu_config config;
    
    config.afs = AFS_16G;
    config.gfs = GFS_2000DPS;
    config.mfs = MFS_16BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);
    
    uint8_t err;
    int16_t ax,ay,az,gx,gy,gz,mx,my,mz;
    
    static int16_t mmx,mmy,mmz;
    while(1)
    {
        err = 0;
    
        err += mpu9250_read_accel_raw(&ax, &ay, &az);
        err += mpu9250_read_gyro_raw(&gx, &gy, &gz);
        err += mpu9250_read_mag_raw(&mx, &my, &mz);
    
        mmx = 0.9*mmx + 0.1*mx;
        mmy = 0.9*mmy + 0.1*my;
        mmz = 0.9*mmz + 0.1*mz;
        
        float angle;
         angle = atan2(mmy, mmx)*57.3;
        if(err)
        {
            printf("!err:%d\r\n", err);
            while(1);
        }

        printf("ax:%05d ay:%05d az:%05d gx:%05d gy:%05d gz:%05d mx:%05d my:%05d mz:%05d %f   \r", ax ,ay, az, gx, gy, gz, mmx, mmy, mmz, angle);  
		GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(5);
    }
}

static int transmit_data_init(void)
{
    DMA_InitTypeDef DMAInitStruct;

    DMAInitStruct.chl = HW_DMA_CH0;
    DMAInitStruct.chlTriggerSource = UART0_TRAN_DMAREQ;
    DMAInitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

    DMAInitStruct.sAddr = 0;
    DMAInitStruct.sAddrIsInc = true;
    DMAInitStruct.sDataWidth = kDMA_DataWidthBit_8;
    DMAInitStruct.sMod = kDMA_ModuloDisable;

    DMAInitStruct.dAddr = (uint32_t)&UART0->D;
    DMAInitStruct.dAddrIsInc = false;
    DMAInitStruct.dDataWidth = kDMA_DataWidthBit_8;
    DMAInitStruct.dMod = kDMA_ModuloDisable;
    DMA_Init(&DMAInitStruct);
    
    DMA_EnableAutoDisableRequest(HW_DMA_CH0, true);
    UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx, true);
    return 0;
}

static int _transmit_data(uint8_t *buf, uint32_t len)
{
    uint32_t remain;
    
    remain = DMA_GetTransferByteCnt(HW_DMA_CH0);
    if(remain)
    {
        return 1;
    }
    
    DMA_CancelTransfer(HW_DMA_CH0);
    DMA_SetSourceAddress(HW_DMA_CH0, (uint32_t)buf);
    DMA_SetTransferByteCnt(HW_DMA_CH0, len);
    DMA_EnableRequest(HW_DMA_CH0);
    return 0;
}


int mpu9250_read_mag_raw2(int16_t* x, int16_t* y, int16_t* z)
{
    static int16_t mmx,mmy,mmz;
    
//    mmx = 0.9*mmx+0.1* mag[0];
//    mmy = 0.9*mmy+0.1* mag[1];
//    mmz = 0.9*mmz+0.1* mag[2];
    mpu9250_read_mag_raw(&mmx, &mmy, &mmz);
    
    *x = cal_data.mxg  *(mmx - cal_data.mxo);
    *y = cal_data.myg *(mmy - cal_data.myo);
    *z = cal_data.mzg *(mmz - cal_data.mzo);
    
    return 0;
}

int mpu9250_read_accel_raw2(int16_t* x, int16_t* y, int16_t* z)
{
    mpu9250_read_accel_raw(x, y, z);
    //*x = accel[0]; 
   // *y = accel[1]; 
   // *z = accel[2]; 
    return 0;
}

int mpu9250_read_gyro_raw2(int16_t* x, int16_t* y, int16_t* z)
{
    mpu9250_read_gyro_raw(x, y, z);
    //*x = gyro[0]; 
   // *y = gyro[1]; 
   // *z = gyro[2]; 
    return 0; 
}

static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = mpu9250_read_accel_raw2,
    .imu_get_gyro = mpu9250_read_gyro_raw2,
    .imu_get_mag = mpu9250_read_mag_raw2,
};

static void send_data_process(imu_float_euler_angle_t *angle, imu_raw_data_t *raw)
{
    static uint8_t buf[64];
    uint32_t len;
    static transmit_user_data data;
    data.trans_accel[0] = raw->ax;
    data.trans_accel[1] = raw->ay;
    data.trans_accel[2] = raw->az;
    data.trans_gyro[0] = raw->gx;
    data.trans_gyro[1] = raw->gy;
    data.trans_gyro[2] = raw->gz;
    data.trans_mag[0] = raw->mx;
    data.trans_mag[1] = raw->my;
    data.trans_mag[2] = raw->mz;
    data.trans_pitch = (int16_t)(angle->imu_pitch*100);
    data.trans_roll = (int16_t)(angle->imu_roll*100);
    data.trans_yaw = 1800 + (int16_t)(angle->imu_yaw*10);
    //data.trans_pressure = pressure;
    
    len = user_data2buffer(&data, buf);
    _transmit_data(buf, len);
}


int main(void)
{
	uint32_t clock;
    uint32_t ret;
    static imu_float_euler_angle_t angle;
    struct mpu_config config;
    
	DelayInit();   
    DelayMs(1);
    
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);    
    printf("CH IMU V1.2\r\n");

    /* MCU driver init */
    I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
    GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 7, 0);
    DelayMs(10);
    
    mpu9250_init(0);
    
    config.afs = AFS_8G;
    config.gfs = GFS_500DPS;
    config.mfs = MFS_14BITS;
    config.aenable_self_test = false;
    config.genable_self_test = false;
    mpu9250_config(&config);

    memcpy(&cal_data, (void*)FLASH_DATA_ADDR, sizeof(cal_data));
    if(cal_data.magic ==  0x5ACB)
    {
        printf("read cal data from flash succ\r\n");
        _print_cal_data(&cal_data);
    }
    else
    {
        printf("read cal data from flash err!\r\n");
        MagnetometerCalibration(&cal_data);
        FLASH_EraseSector(FLASH_DATA_ADDR);
        FLASH_WriteSector(FLASH_DATA_ADDR, (const uint8_t*)&cal_data, sizeof(cal_data));
    }

    //mpu9250_test();
    //dmp_init();

    PIT_QuickInit(HW_PIT_CH1, 1000*1000);
    
    transmit_data_init();
    imu_io_install(&IMU_IOInstallStruct1);
    
    static int time, last_time, load_val;
    static uint32_t fac_us;
    unsigned long sensor_timestamp;
    static unsigned char more;
    long quat[4];
    float dmp_angle[3];
    static float final_yaw;
    
    CLOCK_GetClockFrequency(kBusClock, &fac_us);
    fac_us /= 1000000;

    while(1)
    {
        time = PIT_GetCounterValue(HW_PIT_CH1);
        PIT_ResetCounter(HW_PIT_CH1);
        load_val = PIT_GetCounterValue(HW_PIT_CH1);
        time = load_val - time;
        time /= fac_us;

        imu_get_euler_angle(&imu_angle, &raw_data);

//        mpu_get_compass_reg(mag, NULL);
//        dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);	
//        if (sensors & INV_WXYZ_QUAT )
        {
       //     quat2angle(quat, dmp_angle);
            angle.imu_roll = dmp_angle[0];
            angle.imu_pitch = dmp_angle[1];
            angle.imu_yaw = dmp_angle[2];
            
            angle.imu_pitch = imu_angle.imu_pitch;
            angle.imu_roll = imu_angle.imu_roll;
            angle.imu_yaw = imu_angle.imu_yaw;
        }
        
        halfT = ((float)time)/1000/2000;        
        send_data_process(&angle, &raw_data);
        GPIO_ToggleBit(HW_GPIOC, 3);
    }
}


