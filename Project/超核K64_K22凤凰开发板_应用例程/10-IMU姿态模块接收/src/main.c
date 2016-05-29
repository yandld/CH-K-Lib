#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "imu_rev.h"


/* 超核姿态模块接收试验 */
/* 此实验配合超核串口姿态模块 请将姿态模块连接到 开发板对应引脚。 试验通过普通调试串口显示结果 */

imu_data imu;

/* UART4 串口中断 */
static void UART4_ISR(uint16_t byteReceived)
{
    /* 在串口中断中调用 imu_rev_process 函数, byteReceived是串口接收到的数据 */
    imu_rev_process(byteReceived);
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    /* 打印结果串口 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("IMU data receie, please connect IMU Tx/RX to PC16, PC17...\r\n");
    
    /*姿态模块串口 */
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200); 
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);//功能选择S0
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_OPP);//功能选择S1
    GPIO_WriteBit(HW_GPIOE, 26, 1);//功能选择S0=0
    GPIO_WriteBit(HW_GPIOE, 27, 0);//功能选择S1=1
    
    /* open interrupt */
    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);

    /* 初始化IMU模块 */
    imu_rev_init();
    
    while(1)
    {
        /* 获取数据并打印 */
        imu_get_data(&imu);
        printf("P/R/Y:%2.2f %2.2f %2.2f\r\n", ((float)imu.pitch)/100, ((float)imu.roll)/100, ((float)imu.yaw)/10);
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(50);
    }
}


