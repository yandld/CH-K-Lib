#include "sys.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "clock.h"
#include "cpuidy.h"

#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "shell.h"


extern const cmd_tbl_t CommandFun_Help;

static void Putc(uint8_t data)
{
    UART_WriteByte(HW_UART1, data);
}

static uint8_t Getc(void)
{
    uint8_t ch;
    while(UART_ReadByte(HW_UART1, &ch));
    return ch;
}

shell_io_install_t Shell_IOInstallStruct1 = 
{
	.getc = Getc,
	.putc = Putc,
};

extern const cmd_tbl_t CommandFun_CPU;
extern const cmd_tbl_t CommandFun_Hist;
extern const cmd_tbl_t CommandFun_GPIO;
extern const cmd_tbl_t CommandFun_I2C;
extern const cmd_tbl_t CommandFun_IMUHW;
extern const cmd_tbl_t CommandFun_DELAY;


static mpu6050_device mpu6050_device1;
static hmc5883_device hmc_device;
static bmp180_device bmp180_device1;

#define IMU_TEST_I2C_MAP I2C0_SCL_PE19_SDA_PE18

uint8_t InitSensor(void)
{
    static int init = 0;
    uint8_t ret = 0;
    if(!init)
    {
        ret += mpu6050_init(&mpu6050_device1, IMU_TEST_I2C_MAP, "mpu6050", 96000);
        ret += hmc5883_init(&hmc_device, IMU_TEST_I2C_MAP, "hmc5883", 96000);
        ret += bmp180_init(&bmp180_device1, IMU_TEST_I2C_MAP, "bmp180", 96000);
        init = 1;
    }
    if(ret)
    {
        return ret;
    }
}


int main(void)
{
    uint32_t i;
    uint8_t ret;
    //SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
    SYSTICK_DelayInit();
    UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    GPIO_QuickInit(HW_GPIOA, 1 , kGPIO_Mode_OPP);   //Init LED
    for(i = 0; i < 10; i++)
    {
        GPIO_ToggleBit(HW_GPIOA, 1);
        DelayMs(50);
    }
    ret = InitSensor();
    if(ret)
    {
        printf("Sensor init failed! code:%d\r\n", ret);
    }
    
    shell_io_install(&Shell_IOInstallStruct1);
    shell_register_function(&CommandFun_Help);
    shell_register_function(&CommandFun_GPIO);
    shell_register_function(&CommandFun_I2C);
    shell_register_function(&CommandFun_Hist);
    shell_register_function(&CommandFun_CPU);
    shell_register_function(&CommandFun_IMUHW);
    shell_register_function(&CommandFun_DELAY);

    printf("When you see this string, It means that printf is OK!\r\n");
		
		

    
    printf("HelloWorld\r\n");
    // time measure
    
    while(1);
    

    while(1)
    {
        shell_main_loop("SHELL>>");
    }
}



void assert_failed(char * file, uint32_t line)
{
	//∂œ—‘ ß∞‹ºÏ≤‚
	shell_printf("assert_failed:line:%d %s\r\n",line,file);
	while(1);
}

