//包含超核库所需的头文件
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "stdio.h"
#include "pit.h"
#include "nrf24l01.h"
#include "board.h"

//包含 姿态模块所需的头文件
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "imu.h"
#include "trans.h"
#include "dma.h"
#include "spi_abstraction.h"
#include "spi.h"

static uint8_t NRF2401RXBuffer[32] = "HelloWorld~~";//无线接收数据

static trans_user_data_t send_data;
static imu_float_euler_angle_t angle;
static imu_raw_data_t raw_data;
//实现姿态解算的回调并连接回调
static imu_io_install_t IMU_IOInstallStruct1 = 
{
    .imu_get_accel = mpu6050_read_accel,
    .imu_get_gyro = mpu6050_read_gyro,
    .imu_get_mag = hmc5883_read_data,
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


static void PIT_CH1_ISR(void)
{
    trans_send_pactket(send_data, TRANS_WITH_NRF2401);
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

extern int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance);
int main(void)
{
    int16_t x,y,z;
    uint32_t i;
    uint32_t cnt = 0;
    uint32_t ret;
    uint32_t len;
    int32_t temperature;
    int32_t pressure;
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);  
    UART_QuickInit(BOARD_UART_DEBUG_MAP, 115200);
    printf("UART Init OK!\r\n");
    for(i = 0; i < 10; i++)
    {
        GPIO_ToggleBit(HW_GPIOA, 1);
        DelayMs(50);
    }
    init_sensor();
    //安装IMU 底层驱动函数
    imu_io_install(&IMU_IOInstallStruct1);
    //开PIT0
    PIT_QuickInit(HW_PIT_CH0, 1000*4);
    PIT_CallbackInstall(HW_PIT_CH0, PIT_CH0_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);
    //开PIT1
    PIT_QuickInit(HW_PIT_CH1, 1000*20);
    PIT_CallbackInstall(HW_PIT_CH1, PIT_CH1_ISR);
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF);
    /* 初始化发送单元 */
    trans_init();
    
    /* 初始化NRF的 PinMux*/
    PORT_PinMuxConfig(HW_GPIOC, 5, kPinAlt2); //
    PORT_PinMuxConfig(HW_GPIOC, 6, kPinAlt2); //
    PORT_PinMuxConfig(HW_GPIOC, 7, kPinAlt2); //    

    PORT_PinMuxConfig(HW_GPIOC, BOARD_SPI_CS_PIN, kPinAlt2);
    GPIO_QuickInit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN , kGPIO_Mode_OPP);
    GPIO_QuickInit(BOARD_NRF2401_IRQ_PORT, BOARD_NRF2401_IRQ_PIN , kGPIO_Mode_IPU);
    /* 初始化NRF */
    static struct spi_bus bus;
    ret = kinetis_spi_bus_init(&bus, HW_SPI0);
    nrf24l01_init(&bus, 2);
    if(nrf24l01_probe())
    {
        printf("no nrf24l01 device found\r\n");
    }
    nrf24l01_set_rx_mode();
    while(1)
    {
        if(DMA_IsMajorLoopComplete(HW_DMA_CH1) == 0)
        {
            trans_send_pactket(send_data, TRANS_UART_WITH_DMA);
            //延时1MS以免发的太快上位机受不了
            DelayMs(1);
        }
        
   //     bmp180_read_temperature(&temperature);
   //     bmp180_start_conversion(BMP180_P3_MEASURE);
   //     DelayMs(20);
    //    bmp180_read_pressure(&pressure);
        //printf("t:%d p:%d\r", temperature, pressure);
        if(!nrf24l01_read_packet(NRF2401RXBuffer, &len))
        {
            /* 从2401 接收到数据 */

        }
    }

}


