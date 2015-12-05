#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "imu_rev.h"

/* 超核姿态模块接收试验 */
/* 此实验配合超核串口姿态模块 请将姿态模块连接到 开发板对应引脚。 试验通过普通调试串口显示结果 */

void IMU_REV_Handler(void)
{
    struct imu_data data;
    
    /* get data */
    imu_rev_get_data(&data);
    
    /* print results */
    printf("P/R/Y/P:%05d %05d %05d %05d\r", data.pitch/100, data.roll/100, data.yaw/10, data.presure);
}


static void UART4_ISR(uint16_t byteReceived)
{
    /* hander imu receive */
    imu_rev_process((char)byteReceived, IMU_REV_Interrupt);
}

 struct imu_rev_init installer = 
{
    NULL, /* 如果使用Interrupt模式 则填入NULL即可 */
    IMU_REV_Handler, /* 成功接收一帧后的处理函数 */
};

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
	  GPIO_WriteBit(HW_GPIOE, 26, 0);//功能选择S0=0
	  GPIO_WriteBit(HW_GPIOE, 27, 1);//功能选择S1=1
    /* open interrupt */
    UART_CallbackRxInstall(HW_UART4, UART4_ISR);
    UART_ITDMAConfig(HW_UART4, kUART_IT_Rx, true);

    /* install functions */
    imu_rev_init(&installer);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);
    }
}


