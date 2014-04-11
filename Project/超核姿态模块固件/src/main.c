//包含超核库所需的头文件
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "stdio.h"
#include "pit.h"
#include "24l01.h"
#include "board.h"

//包含 姿态模块所需的头文件
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "imu.h"
#include "trans.h"
#include "dma.h"



#if 0

static trans_user_data_t send_data;
static uint8_t NRF2401RXBuffer[32];//无线接收数据
imu_float_euler_angle_t angle;
imu_raw_data_t raw_data;
//实现姿态解算的回调并连接回调


static uint32_t imu_get_mag(int16_t * mx, int16_t * my, int16_t *mz)
{
    hmc_device.read_data(&hmc_device, mx, my, mz);
    return 0;
}

static uint32_t imu_get_accel(int16_t * ax, int16_t * ay, int16_t *az)
{
    mpu6050_device1.read_accel(&mpu6050_device1, ax,ay,az);
    return 0;  
}

static uint32_t imu_get_gyro(int16_t * gx, int16_t * gy, int16_t * gz)
{
    mpu6050_device1.read_gyro(&mpu6050_device1, gx,gy,gz);
    return 0;
}

static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = imu_get_accel,
    .imu_get_gyro = imu_get_gyro,
    .imu_get_mag = imu_get_mag,
};


//中断服务
static void PIT_CH0_ISR(void)
{
    //获取欧拉角 获取原始角度
    imu_get_euler_angle(&angle, &raw_data);
    send_data.trans_accel[0] = raw_data.ax;
    send_data.trans_accel[1] = raw_data.ay;
    send_data.trans_accel[2] = raw_data.az;
    send_data.trans_gyro[0] = raw_data.gx;
    send_data.trans_gyro[1] = raw_data.gy;
    send_data.trans_gyro[2] = raw_data.gz;
    send_data.trans_mag[0] = raw_data.mx;
    send_data.trans_mag[1] = raw_data.my;
    send_data.trans_mag[2] = raw_data.mz;     
    send_data.trans_pitch = (int16_t)angle.imu_pitch*100;
    send_data.trans_roll = (int16_t)angle.imu_roll*100;
    send_data.trans_yaw = (int16_t)angle.imu_yaw*10;
}



#endif 

static void PIT_CH1_ISR(void)
{
//    trans_send_pactket(send_data, TRANS_WITH_NRF2401);
    GPIO_ToggleBit(HW_GPIOA, 1);
}

int kinetis_i2c_bus_init(struct i2c_bus* bus, uint32_t instance);

int init_sensor(void)
{
    uint32_t ret;
    static struct i2c_bus bus;
    ret = kinetis_i2c_bus_init(&bus, BOARD_I2C_INSTANCE);
    /* enable pinmux */
    PORT_PinMuxConfig(HW_GPIOE, 18, kPinAlt4);
    PORT_PinMuxConfig(HW_GPIOE, 19, kPinAlt4);
    ret = mpu6050_init(&bus);
    ret = hmc5883_init(&bus);
    ret = bmp180_init(&bus);
    ret = mpu6050_probe();
    if(ret)
    {
        printf("mpu6050 failed:%d\r\n", ret);
    }
    ret = hmc5883_probe();
    if(ret)
    {
        printf("hmc5883 failed:%d\r\n", ret);
    }
    ret = bmp180_probe();
    if(ret)
    {
        printf("bmp180 failed:%d\r\n", ret);
    }
    return ret;
}


int main(void)
{
    int16_t x,y,z;
    uint32_t i;
    uint32_t ret;
    int32_t temperature;
    int32_t pressure;
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);  
    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("UART Init OK!\r\n");
    //开PIT0
    PIT_QuickInit(HW_PIT_CH0, 1000*4);
    //PIT_CallbackInstall(HW_PIT_CH0, PIT_CH0_ISR);
    //PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);
    //开PIT1
    PIT_QuickInit(HW_PIT_CH1, 1000*20);
    PIT_CallbackInstall(HW_PIT_CH1, PIT_CH1_ISR);
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF);
    
    init_sensor();
    
    while(1)
    {
        //mpu6050_read_gyro(&x, &y, &z);
        
        bmp180_start_conversion(BMP180_T_MEASURE);
        DelayMs(20);
       // mpu6050_read_data(&x, &y, &z);
       // printf("x:%04d, y:%04d, z:%04d  \r", x, y, z);
        bmp180_read_temperature(&temperature);
        bmp180_start_conversion(BMP180_P3_MEASURE);
        DelayMs(20);
        bmp180_read_pressure(&pressure);
        printf("t:%d p:%d\r", temperature, pressure);
        
        DelayMs(10);
        
    }
    
    
    
    
    
    
    
    
    
    
   #if 0
    // 初始化LED
    ret = NRF2401_Init();
    if(ret)
    {
        printf("NRF2401 ERROR\r\n");
        DelayMs(300);
    }
    for(i = 0; i < 10; i++)
    {
        GPIO_ToggleBit(HW_GPIOA, 1);
        DelayMs(50);
    }
    //初始化传感器
    ret = InitSensor();
    if(ret)
    {
        printf("Sensor init failed! code:%d\r\n", ret);
    }
    //安装IMU 底层驱动函数
    imu_io_install(&IMU_IOInstallStruct1);
    //初始化发送器
    trans_init();

    
    while(1)
    {
        //压力采集程序 由于上位机不传送压力，所以先注释掉
        /*
        while(1)
        {
            bmp180_start_conversion(&bmp180_device1, BMP180_T_MEASURE);
            DelayMs(50);
            bmp180_read_temperature(&bmp180_device1, &temperature);
            bmp180_start_conversion(&bmp180_device1, BMP180_P3_MEASURE);
            DelayMs(50);
            bmp180_read_pressure(&bmp180_device1, &pressure);
            printf("T:%05d P:%05d\r\n", temperature, pressure);
        }
        */
        //如果DMA空闲 则 启动发送数据
        if(DMA_IsTransferComplete(HW_DMA_CH1) == 0)
        {
            trans_send_pactket(send_data, TRANS_UART_WITH_DMA);
            //延时1MS以免发的太快上位机受不了
            DelayMs(1);
        }
        NRF2401_SetRXMode(); //设置为接收模式
        if(NRF2401_RecPacket(NRF2401RXBuffer) != NRF_OK) //接收到了数据
        {
            //目前这个版本不做任何处理
        }	
    }
    #endif
}


