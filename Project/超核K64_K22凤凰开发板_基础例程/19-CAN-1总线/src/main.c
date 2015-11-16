#include "gpio.h"
#include "uart.h"
#include "can.h"



#define CAN_TX_ID  0x10
#define CAN_RX_ID  0x56

/*
     实验名称：CAN通信测试
     实验平台：凤凰开发板
     板载芯片：MK64DN512ZVQ10
 实验效果：使用CAN0模块的3号邮箱采用中断的方式接收来自0x56的数据
         使用2号邮箱向0x10地址的设备发生数据，时间间隔是500毫秒 
*/

/* CAN通信 中断回调函数，在中断中处理接收到的数据 */
void CAN_ISR(void)
{
    uint8_t buf[8];
    uint8_t len;
    uint32_t id;
    if(CAN_ReadData(HW_CAN0, 3, &id, buf, &len) == 0)
    {
        printf("DataReceived:id 0x%x ", (id>>CAN_ID_STD_SHIFT));
        while(len--)
        {
            printf("[%d]:0x%X ", len, buf[len]);
        }
        printf("\r\n");
    }
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("CAN test\r\n");
    /* 初始化 CAN 使用CAN1模块的PTE24/25引脚，通信速度为125k*/
    CAN_QuickInit(CAN0_TX_PB18_RX_PB19, 125*1000);
    
    /* 设置接收中断 安装回调函数 */
    CAN_CallbackInstall(HW_CAN0, CAN_ISR);
    /* 开启CAN通信中断接收功能，3号邮箱 */
    CAN_ITDMAConfig(HW_CAN0,3, kCAN_IT_RX);
    
    /* 设置 3号邮箱为CAN接收邮箱 */
    CAN_SetRxMB(HW_CAN0, 3, CAN_RX_ID);
    while(1)
    {
        /* 使用邮箱2 发送ID:0x10 发送 "CAN TEST" */
        CAN_WriteData(HW_CAN0, 2, CAN_TX_ID, (uint8_t *)"CAN TEST", 8); /* 使用邮箱2 发送ID:0x10 发送 "CAN TEST" */
        DelayMs(500);
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
}


