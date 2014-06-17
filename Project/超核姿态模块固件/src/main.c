//包含超核库所需的头文件
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "stdio.h"
#include "pit.h"
#include "nrf24l01.h"
#include "wdog.h"
#include "board.h"
#include "protocol.h"

//包含 姿态模块所需的头文件
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "imu.h"
#include "trans.h"
#include "dma.h"
#include "spi_abstraction.h"
#include "spi.h"

#include "ssd.h"

/* 校准数据 */
struct calibration_data
{
    int  flag;
    float meg_x_gain;
    float meg_y_gain;
    float meg_z_gain;
    int   meg_x_off;
    int   meg_y_off;
    int   meg_z_off;
};

static struct calibration_data cal_data;
//static uint8_t NRF2401RXBuffer[32] = "HelloWorld~~";//无线接收数据
static imu_float_euler_angle_t angle;
static imu_raw_data_t raw_data;

int hmc5883_read_data2(int16_t* x, int16_t* y, int16_t* z)
{
    int r;
    int16_t xraw,yraw,zraw;
    r = hmc5883_read_data(&xraw, &yraw, &zraw);
    if(!r)
    {
        *x = cal_data.meg_x_gain  *(xraw - cal_data.meg_x_off);
        *y = cal_data.meg_y_gain *(yraw - cal_data.meg_y_off);
        *z = cal_data.meg_z_gain *(zraw - cal_data.meg_z_off);
    }
    return r;
}
 
//实现姿态解算的回调并连接回调
static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = mpu6050_read_accel,
    .imu_get_gyro = mpu6050_read_gyro,
    .imu_get_mag = hmc5883_read_data2,
};


static void PIT_CH1_ISR(void)
{
    uint32_t len;
    static transmit_user_data send_data;
    static uint8_t buf[64];
    send_data.trans_accel[0] = raw_data.ax;
    send_data.trans_accel[1] = raw_data.ay;
    send_data.trans_accel[2] = raw_data.az;
    send_data.trans_gyro[0] = raw_data.gx;
    send_data.trans_gyro[1] = raw_data.gy;
    send_data.trans_gyro[2] = raw_data.gz;
    send_data.trans_mag[0] = raw_data.mx;
    send_data.trans_mag[1] = raw_data.my;
    send_data.trans_mag[2] = raw_data.mz;
    send_data.trans_pitch = (int16_t)(angle.imu_pitch*100);
    send_data.trans_roll = (int16_t)(angle.imu_roll*100);
    send_data.trans_yaw = 1800 + (int16_t)(angle.imu_yaw*10);
    
    len = user_data2buffer(&send_data, buf);
    trans_start_send_data(buf, len);
    
    GPIO_ToggleBit(HW_GPIOA, 1);
    WDOG_Refresh();
   // trans_send_pactket(send_data, TRANS_WITH_NRF2401);
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
    PORT_PinOpenDrainConfig(HW_GPIOE, 18, ENABLE);
    PORT_PinOpenDrainConfig(HW_GPIOE, 19, ENABLE);
    PORT_PinPullConfig(HW_GPIOE, 18, kPullUp);
    PORT_PinPullConfig(HW_GPIOE, 19, kPullUp);
    
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
    printf("start calibration, read flash...\r\n");
    SSD_Read(cal, sizeof(struct calibration_data));
    if(cal->flag == 0x5A)
    {
        printf("flash data read succ\r\n");
        printf("Gain X:%f Y:%f Z:%f\r\n", cal->meg_x_gain, cal->meg_y_gain, cal->meg_z_gain);
        printf("Off X:%d Y:%d Z:%d\r\n", cal->meg_x_off, cal->meg_y_off, cal->meg_z_off);
        return;
    }
    for(i=0;i<1000;i++)
    {
        r = hmc5883_read_data(&x, &y, &z);
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
    cal->meg_x_off = (xmax + xmin) / 2;
    cal->meg_x_gain=1;
    cal->meg_y_off = (ymax + ymin) / 2;
    cal->meg_y_gain= (float)(xmax - xmin) / (float)(ymax -ymin);
    cal->meg_z_off = (zmax + zmin) / 2;
    cal->meg_z_gain= (float)(xmax - xmin) / (float)(zmax -zmin);
    /* see if we get data correct */
    if((xmax < 300) || (ymax < 300) || (zmax < 300) || (cal->meg_y_gain < 0.8) || (cal->meg_z_gain < 0.8))
    {
        printf("cal failed, setting to default param\r\n");
        /* inject with default data */
        cal->meg_x_off = 0;
        cal->meg_y_off = 0;
        cal->meg_z_off = 0;
        cal->meg_x_gain = 1;
        cal->meg_y_gain = 1;
        cal->meg_z_gain = 1;
    }
    printf("Gain X:%f Y:%f Z:%f\r\n", cal->meg_x_gain, cal->meg_y_gain, cal->meg_z_gain);
    printf("Off X:%d Y:%d Z:%d\r\n", cal->meg_x_off, cal->meg_y_off, cal->meg_z_off);
    cal->flag = 0x5A;
    SSD_Write(cal, sizeof(struct calibration_data));
}

int main(void)
{
    int i;
    uint32_t ret;
    uint32_t uart_instance;
    //int32_t temperature;
    //int32_t pressure;
    /* basic hardware */
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);  
    uart_instance = UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    
    /* force I2C bus to unlock */
    GPIO_QuickInit(HW_GPIOE, 18, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 19, kGPIO_Mode_OPP);
    for(i=0;i<18;i++)
    {
        PEout(19) = !PEout(19); //SCL:E19
        DelayMs(1);
    }
    
    /* show power on */
    for(i=0;i<6;i++)
    {
        PAout(1) = !PAout(1);
        DelayMs(30);
    }
    
    /* flash operation */
    SSDInit();
    
    /* init sensor */
    init_sensor();
    
    /* install imu raw date callback */
    imu_io_install(&IMU_IOInstallStruct1);
    
    /* read meg cal data */
    MagnetometerCalibration(&cal_data);
    
    /* init transfer */
    trans_init(HW_DMA_CH2, uart_instance);
    
    WDOG_QuickInit(100);
    
    PIT_QuickInit(HW_PIT_CH1, 1000*20);
    PIT_CallbackInstall(HW_PIT_CH1, PIT_CH1_ISR);
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF);
    
    /* 初始化NRF的 PinMux*/
    PORT_PinMuxConfig(HW_GPIOC, 5, kPinAlt2); //
    PORT_PinMuxConfig(HW_GPIOC, 6, kPinAlt2); //
    PORT_PinMuxConfig(HW_GPIOC, 7, kPinAlt2); //    

    PORT_PinMuxConfig(HW_GPIOC, BOARD_SPI_CS_PIN, kPinAlt2);
    GPIO_QuickInit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN , kGPIO_Mode_OPP);
    GPIO_QuickInit(BOARD_NRF2401_IRQ_PORT, BOARD_NRF2401_IRQ_PIN , kGPIO_Mode_IPU);
    /* 初始化NRF */
   // static struct spi_bus bus;
   // ret = kinetis_spi_bus_init(&bus, HW_SPI0);
    //nrf24l01_init(&bus, 2);
    // if(nrf24l01_probe())
    //{
    //     printf("no nrf24l01 device found\r\n");
    //}
    //nrf24l01_set_rx_mode();
    while(1)
    {
        //获取欧拉角 获取原始角度
        DisableInterrupts();
        ret = imu_get_euler_angle(&angle, &raw_data);
        EnableInterrupts();
        if(ret)
        {
            DisableInterrupts();
            printf("imu module fail!\r\n");
            while(1);
        }
        DelayMs(5);
        #if 0
        printf("P:%4d R:%4d Y:%4d  \r", (int)angle.imu_pitch, (int)angle.imu_roll, (int)angle.imu_yaw);
        bmp180_read_temperature(&temperature);
        bmp180_start_conversion(BMP180_P3_MEASURE);
        DelayMs(20);
        bmp180_read_pressure(&pressure);
        printf("t:%d p:%d\r", temperature, pressure);
          if(!nrf24l01_read_packet(NRF2401RXBuffer, &len))
        {
            /* 从2401 接收到数据 */

        }
        #endif
    }
}


