#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"
#include "pit.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* 可用的FTM 正交解码 通道有: */
/*
 FTM1_QD_PHA_PA08_PHB_PA09       
 FTM1_QD_PHA_PA12_PHB_PA13       
 FTM1_QD_PHA_PB00_PHB_PB01       
 FTM2_QD_PHA_PA10_PHB_PA11       
 FTM2_QD_PHA_PB18_PHB_PB19       
*/

/*
     实验名称：FTM正交解码
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：使用FTM1模块的PTA8、PTA9引脚进行正交解码
      通过串口将正交解码的数据发送出去
      小灯周期性闪烁，闪烁时间间隔500ms     
*/

void PIT_ISR(void)
{
    int value; /* 记录正交脉冲个数 */
    uint8_t dir; /* 记录编码器旋转方向1 */
    /* 获取正交解码数据 */
    FTM_QD_GetData(HW_FTM1, &value, &dir);
    printf("value:%d dir:%d  \r", value, dir);
    FTM_QD_ClearCount(HW_FTM1);  
}

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP); /* LED */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /*初始化调试串口 */

    printf("PWM_QD test, connect encoder for PA08 PA09\r\n");
     
    /* 初始化正交解码通道 */
    FTM_QD_InitTypeDef FTM_QD_InitStruct1 = {0};
    FTM_QD_InitStruct1.instance = HW_FTM1;  //使用FTM1模块
    FTM_QD_InitStruct1.mode = kQD_PHABEncoding; /* 编码器类型 普通2线 AB相编码器 */
    FTM_QD_InitStruct1.PHA_Polarity = kFTM_QD_NormalPolarity; /* 正常极性 */
    FTM_QD_InitStruct1.PHB_Polarity = kFTM_QD_NormalPolarity;
    FTM_QD_Init(&FTM_QD_InitStruct1);
    
    /* 初始化正交解码引脚 */
    PORT_PinMuxConfig(HW_GPIOA, 8, kPinAlt6);
    PORT_PinMuxConfig(HW_GPIOA, 9, kPinAlt6);
    
    /* 开启PIT中断 */
    PIT_QuickInit(HW_PIT_CH0, 1000*10);
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


