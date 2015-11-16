#include "gpio.h"
#include "uart.h"
#include "can.h"



#define CAN_TX_MB  12
#define CAN_TX_ID  0x10

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
    static uint32_t cnt;
    uint8_t buf[8];
    uint8_t len;
    uint32_t id;
    
    /* FIFO */
    if(CAN_IsRxFIFOEnable(HW_CAN0))
    {
        if(CAN0->IFLAG1 & (1 << CAN_RX_FIFO_MB))
        {
            if(CAN_ReadFIFO(HW_CAN0, &id, buf, &len) == 0)
            {
                printf("DataReceived:%d ", cnt++);
                while(len--)
                {
                    printf("[%d]:0x%X ", len, buf[len]);
                }
                printf("\r\n");
            }
        }
    }
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("CAN test\r\n");
    CAN_QuickInit(CAN0_TX_PB18_RX_PB19, 125*1000);
    
    /* FIFO deep is 0+6 = 6 
    Once FIFO is enabled, MB0-5 cannot used as normal MB, MB0-5 used as RxFIFO and they automaically configed as Rx MB 
    */
    CAN_SetRxFIFO(HW_CAN0);
    CAN_CallbackInstall(HW_CAN0, CAN_ISR);
    CAN_ITDMAConfig(HW_CAN0, CAN_RX_FIFO_MB, kCAN_IT_RX);
    
    while(1)
    {
        CAN_WriteData(HW_CAN0, CAN_TX_MB, CAN_TX_ID, (uint8_t *)"CAN TEST", 8); /* 使用邮箱2 发送ID:0x10 发送 "CAN TEST" */
        DelayMs(500);
        GPIO_ToggleBit(HW_GPIOA, 9);
    }
}


