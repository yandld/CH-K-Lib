#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "can.h"

uint8_t CANRXBuffer[8];//??can????8????
uint8_t CANTXBuffer[8];//??can?????
uint32_t CAN_Data_Len;  //???????????????

int main(void)
{
    uint32_t i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* µ÷ÊÔ´®¿Ú */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    CAN_QuickInit(CAN1_TX_PE24_RX_PE25, kCAN_Baudrate_125K);
    //CAN_EnableRev(CAN1, CAN_DEFAULT_RXMSGBOXINDEX, CAN_DEFAULT_RXID);
    for(i=0;i<8;i++)
    CANTXBuffer[i]= i;
    CAN_Data_Len = 0;
    while(1)
    {
        /* ?CAN???????? */
        i = CAN_SendData(CAN1, 2, 8, CANTXBuffer,8);
       if(i == 0)
       {
           printf("CAN send succ\r\n");
       }
       else
       {
           printf("CAN send err!%d\r\n",i);
       }
        DelayMs(500);
    }
}


