#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pdb.h"
/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */


void PDB_ISR(void)
{
    printf("!!!\r\n");
}
 
int main(void)
{
    DelayInit();
    /* 初始化一个引脚 设置为推挽输出 */
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* 初始化一个串口 使用UART0端口的PTD6引脚和PTD7引脚作为接收和发送，默认设置 baud 115200 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("HelloWorld!\r\n");
    
    PDB_InitTypeDef PDB_InitStruct1;
    PDB_InitStruct1.inputTrigSource = 15;
    PDB_InitStruct1.isContinuesMode = true;
    PDB_InitStruct1.srcClock = 96000000;
    PDB_Init(&PDB_InitStruct1);
    PDB_CallbackInstall(PDB_ISR);
    PDB_ITDMAConfig(kPDB_IT_CF, true);
    
    PDB_SoftwareTrigger();
    
    while(1)
    {
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


