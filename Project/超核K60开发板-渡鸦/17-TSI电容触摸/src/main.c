#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "tsi.h"

/* TSI 中断回调函数 */
void TSI_ISR(uint32_t outOfRangeArray)
{
    static uint32_t last_chl_array;
    uint32_t value;
    value = outOfRangeArray^last_chl_array;
    /* 点亮对应的小灯 */
    if(value & outOfRangeArray & (1<<HW_TSI_CH1))
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
    if(value & outOfRangeArray & (1<<HW_TSI_CH2))
    {
        GPIO_ToggleBit(HW_GPIOE, 12);
    }
    if(value & outOfRangeArray & (1<<HW_TSI_CH3))
    {
        GPIO_ToggleBit(HW_GPIOE, 11);
    }
    if(value & outOfRangeArray & (1<<HW_TSI_CH4))
    {
        GPIO_ToggleBit(HW_GPIOE, 7);
    }
    last_chl_array = outOfRangeArray;
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


