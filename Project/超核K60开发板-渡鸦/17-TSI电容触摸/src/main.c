#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "tsi.h"

/* TSI 中断回调函数 */

/* TSI计数阀值 */
#define TSI_THRESHOLD   (2000)

void TSI_ISR(void)
{
    uint32_t i;
    static bool inital_it = true;
    static int32_t inital_value[16];
    int32_t tsi_value[16];
    uint32_t out_of_rang_flag = 0;
    static uint32_t last_flag = 0;
    uint32_t value;
    TSI_ITDMAConfig(kTSI_IT_Disable);
    if(inital_it)
    {
        /* 记录采集第一次的初始值 */
        for(i = 1; i < 5; i++)
        {
            inital_value[i] = TSI_GetCounter(i);
        }
        inital_it = false;        
    }
    for(i = 1; i < 5; i++)
    {
        tsi_value[i] = TSI_GetCounter(i);
    }
    /* 判断是否超出范围 */
    for(i = 1; i < 5; i++)
    {
        if(ABS(tsi_value[i] - inital_value[i]) > TSI_THRESHOLD)
        {
            out_of_rang_flag |= (1<<i);
        }
        else
        {
            out_of_rang_flag &= ~(1<<i);
        }
    }
    
    value = out_of_rang_flag ^ last_flag;
    if(value & out_of_rang_flag & (1<<1))
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
    if(value & out_of_rang_flag & (1<<2))
    {
        GPIO_ToggleBit(HW_GPIOE, 12);
    }
    if(value & out_of_rang_flag & (1<<3))
    {
        GPIO_ToggleBit(HW_GPIOE, 11);
    }
    if(value & out_of_rang_flag & (1<<4))
    {
        GPIO_ToggleBit(HW_GPIOE, 7);
    }
    last_flag = out_of_rang_flag;
    /* 开启中断 */
    TSI_ITDMAConfig(kTSI_IT_EndOfScan);

}

int main(void)
{
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200); /* 为了代码简单 这里直接调用快速初始化了 */
    
    printf("TSI test\r\n");
    
    /* 使能4个TSI 触摸引脚 注意 TSI上电需要校准 下载后 必须拔掉 JLINK 然后 复位 */
    TSI_QuickInit(TSI0_CH1_PA00);
    TSI_QuickInit(TSI0_CH2_PA01);
    TSI_QuickInit(TSI0_CH3_PA02);
    TSI_QuickInit(TSI0_CH4_PA03);
    /* 安装 TSI中断回调函数 */
    TSI_CallbackInstall(TSI_ISR);
    /* 开启 TSI 扫描完成中断 */
    TSI_ITDMAConfig(kTSI_IT_EndOfScan);
    
    while(1)
    {
        /* 等待中断 */
    }
}


