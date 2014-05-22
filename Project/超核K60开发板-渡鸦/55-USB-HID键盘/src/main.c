#include "gpio.h"
#include "common.h"
#include "uart.h"

//usb
#include "usb.h"
#include "usb_hid.h"

/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/

/* 使用USB时 FLL/PLL输出时钟必须为96M  默认配置是 /2 = 48M  在 SIM->CLKDIV2 */
 
int main(void)
{
    uint8_t FnKey = 0;
    uint8_t Keybuf[6] = {0,0,0,0,0,0};
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOA,  4, kGPIO_Mode_IPU);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    if(clock != 96000000)
    {
        printf("USB clock must be 96000000/2\r\nUSB may be malfunction!\r\n");
        printf("change system_xxxx.c to select correct clock setttings\r\n");
    }
    
    UART_printf("Waitting for USB connect ...\r\n");
    USB_Init(); //初始化USB设备 
    USB_WaitDeviceEnumed(); //等待USB枚举成功
    UART_printf("USB connected!\r\n");
    while(1)
    {
        if(PEin(26) == 0) /* 按键按下 */
        {
            Keybuf[0] = 4;  /* A 的USBHIDKeyCode */
        }
        USB_HID_SetKeyBoard(FnKey, Keybuf);
        Keybuf[0] = 0;
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOE, 6);
        HID_Proc(); //执行HID进程
    }
}


