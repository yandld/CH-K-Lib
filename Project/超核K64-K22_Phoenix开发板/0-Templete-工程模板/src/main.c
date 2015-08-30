#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "IS61WV25616.h"
#include "ili9320.h"
#include "wm8960.h"
#include "i2s.h"

#include "pcm_data.h"

typedef enum
{
    kUART_SEL_RS485, 
    kUART_SEL_ESP8266,
    kUART_SEL_IMU,
    kUART_SEL_GPS,

}Sel_t;

void SelectUART(Sel_t sel)
{
    switch(sel)
    {
        case kUART_SEL_RS485:
            PEout(24) = 0;
            PEout(25) = 0;
            break;
        case kUART_SEL_IMU:
            PEout(24) = 0;
            PEout(25) = 1;
            break;
        case kUART_SEL_GPS:
            PEout(24) = 1;
            PEout(25) = 0;
            break;
        case kUART_SEL_ESP8266:
            PEout(24) = 1;
            PEout(25) = 1;
            break;
    }
}


void UART4_ISR(uint16_t data)
{
 //   printf("4data:%d\r\n", data+1);
   // UART_WriteByte(4, data+1);
    
}

void UART0_ISR(uint16_t data)
{
    //printf("data:%c\r\n", data);
    
    
}


int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    /* UART SEL */
    GPIO_QuickInit(HW_GPIOE, 24, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 25, kGPIO_Mode_OPP);
    
    GPIO_QuickInit(HW_GPIOA, 25, kGPIO_Mode_OPP);
    
    /* WIFI RST */
    //GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP);
    //PEout(8) = 0;
   // DelayMs(10);
   // PEout(8) = 1;
    
    /* WIFI MODE */
  //  GPIO_QuickInit(HW_GPIOE, 9, kGPIO_Mode_OPP);
  //  PEout(9) = 0;
    
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_ITDMAConfig(0, kUART_IT_Rx, true);
    UART_CallbackRxInstall(0, UART0_ISR);
    
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200);
    UART_ITDMAConfig(4, kUART_IT_Rx, true);
    UART_CallbackRxInstall(4, UART4_ISR);
    
    ili9320_init();
    ili9320_clear(BLUE);
    
    printf("HelloWorld!\r\n");
  //  I2C_QuickInit(I2C0_SCL_PB02_SDA_PB03, 100*1000);
  //  wm8960_init(0);
   // wm8960_format_config(44100, 16);
    I2S_InitTypeDef Init;
    Init.instance = 0;
    Init.isStereo = false;
    Init.isMaster = true;
    Init.protocol = kSaiBusI2SLeft;
    Init.sampleBit = 16;
    Init.sampleRate = 11025;
    Init.chl = 0;
//    I2S_Init(&Init);
//    
//    
//    /* i2s pinmux */
//    PORT_PinMuxConfig(HW_GPIOE, 6, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOE, 7, kPinAlt4);
//    PORT_PinMuxConfig(HW_GPIOE, 12, kPinAlt4); 
//    PORT_PinMuxConfig(HW_GPIOE, 11, kPinAlt4); 
//    PORT_PinMuxConfig(HW_GPIOE, 10, kPinAlt4); 
    
    
    SelectUART(kUART_SEL_RS485);
    
    while(1)
    {
       // printf("kUART_SEL_RS485\r\n");
        
        GPIO_WriteBit(HW_GPIOA, 25, 1);
        UART_WriteByte(4, 'A');
      //  GPIO_WriteBit(HW_GPIOA, 25, 0);
//        UART_WriteByte(4, '$');
//        UART_WriteByte(4, 'P');
//        UART_WriteByte(4, 'D');
//        UART_WriteByte(4, 'T');
//        UART_WriteByte(4, 'I');
//        UART_WriteByte(4, 'N');
//        UART_WriteByte(4, 'F');
//        UART_WriteByte(4, 'O');
//        UART_WriteByte(4, '\r');
//        UART_WriteByte(4, '\n');
        DelayMs(100);
        GPIO_ToggleBit(HW_GPIOA, 9);
    
    }
    

    //SRAM_Init();
   // SRAM_SelfTest();

    while(1)
    {
        ili9320_clear(RED);
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(200);
    } 
}


