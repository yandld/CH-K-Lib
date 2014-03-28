#include "gpio.h"
#include "common.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 startup_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* GPIO 工作模式 */
/*
 *         @arg kGPIO_Mode_IFT :悬空输入
 *         @arg kGPIO_Mode_IPD :下拉输入
 *         @arg kGPIO_Mode_IPU :上拉输入
 *         @arg kGPIO_Mode_OOD :开漏输出 如果不清楚这2种输出的区别请 百度..
 *         @arg kGPIO_Mode_OPP :推挽输出
 */
 
int main(void)
{
    bool val[3];
    /* 初始化Delay */
    DelayInit();
    /* 将GPIO设置为输入模式 芯片内部自动配置上拉电阻 */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOA,  4, kGPIO_Mode_IPU);
    /* 设置为输出 */
    GPIO_QuickInit(HW_GPIOE,  7, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE,  6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE,  11, kGPIO_Mode_OPP);
    while(1)
    {
        /* 不断读取 引脚电平状态 返回0 或 1*/
        val[0] = GPIO_ReadBit(HW_GPIOE, 26);
        val[1] = GPIO_ReadBit(HW_GPIOE, 27);
        val[2] = GPIO_ReadBit(HW_GPIOA, 4);
        /* 将返回的电平设置到另外一个引脚上 */
        GPIO_WriteBit(HW_GPIOE, 11, val[0]);
        GPIO_WriteBit(HW_GPIOE, 7, val[1]);
        GPIO_WriteBit(HW_GPIOE, 6, val[2]);
    }
}


