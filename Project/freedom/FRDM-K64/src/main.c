#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"

#include "DataScope_DP.h"
#include "math.h"
 /*
     实验名称：UART打印信息
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用串口UART将芯片的出厂信息在芯片上电后发送出去
        发送完毕后，进入while中，执行小灯闪烁效果
*/
 
int main(void)
{
    int cnt;
    int i;
    float num;
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PB16_TX_PB17, 115200);
    /* 打印芯片信息 */
    printf("%s - %dP\r\n", CPUIDY_GetFamID(), CPUIDY_GetPinCount());
    /* 打印时钟频率 */
//  CLOCK_GetClockFrequency(kCoreClock, &clock);
//    printf("core clock:%dHz\r\n", clock);
//    CLOCK_GetClockFrequency(kBusClock, &clock);
 //   printf("bus clock:%dHz\r\n", clock);

    while(1)
    { 
        DataScope_Get_Channel_Data( sin(num+=0.5) , 1 ); //将数据 1.0  写入通道 1
        DataScope_Get_Channel_Data( 2.0 , 2 ); //将数据 2.0  写入通道 2
        DataScope_Get_Channel_Data( 3.0 , 3 ); //将数据 3.0  写入通道 3
        DataScope_Get_Channel_Data( 4.0 , 4 ); //将数据 4.0  写入通道 4
        DataScope_Get_Channel_Data( 5.0 , 5 ); //将数据 5.0  写入通道 5
        DataScope_Get_Channel_Data( 6.0 , 6 ); //将数据 6.0  写入通道 6
        DataScope_Get_Channel_Data( 7.0 , 7 ); //将数据 7.0  写入通道 7
        DataScope_Get_Channel_Data( 8.0 , 8 ); //将数据 8.0  写入通道 8
        DataScope_Get_Channel_Data( 9.0 , 9 ); //将数据 9.0  写入通道 9
        DataScope_Get_Channel_Data( 10.0 , 10); //将数据 10.0 写入通道 10
      
        cnt = DataScope_Data_Generate(10); //生成10个通道的 格式化帧数据，返回帧数据长度
		
        for( i = 0 ; i < cnt; i++)  //循环发送,直到发送完毕   
        {
            UART_WriteByte(HW_UART0, DataScope_OutPut_Buffer[i]);  
        }
    //    GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(1);
        }
}


