#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"


#include "w25qxx.h"


struct cmd_t
{
    char		*name;
    uint8_t		maxargs;
    int         (*cmd)(int argc, char * const argv[]);
    char		*help;
};
    



//int cmd_gpio(int argc, char * const argv[]);
//int cmd_dma(int argc, char * const argv[]);
//int cmd_i2c(int argc, char * const argv[]);
//int cmd_spi(int argc, char * const argv[]);
//int cmd_adc(int argc, char * const argv[]);
//int cmd_usb(int argc, char * const argv[]);

//static struct cmd_t cmd[]=
//{
//    "LED", 5, cmd_gpio,     "do led test",
//    "I2C", 5, cmd_i2c,      "do i2c test",
//    "DMA", 5, cmd_dma,      "do dma test", 
//    "ADC", 5, cmd_adc,      "do adc test",
//    "USB", 5, cmd_usb,      "do usb test",
//    "SPI", 5, cmd_spi,      "do spi test", 
//   "LOOP", 5, NULL,         "do loop test",
//};

#define LED0_PORT   HW_GPIOC
#define LED0_PIN    3
#define LED1_PORT   HW_GPIOA
#define LED1_PIN    1
#define LED2_PORT   HW_GPIOE
#define LED2_PIN    0
#define LED3_PORT   HW_GPIOA
#define LED3_PIN    2

int main(void)
{
    int i, ret;
    
    GPIO_Init(HW_GPIOA, PIN1|PIN2|PIN31, kGPIO_Mode_OPP);
    
    /* UART */
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    LPUART_ITDMAConfig(HW_LPUART0, kUART_IT_Rx, true);
    

    printf("core clock:%dHz\r\n", GetClock(kCoreClock));
    printf("bus clock:%dHz\r\n", GetClock(kBusClock));   
//    /* do test */
//    for(i=0; i<ARRAY_SIZE(cmd); i++)
//    {
//        printf("processing %s cmd...\r\n", cmd[i].name);
//        if(cmd[i].cmd)
//        {
//            ret = cmd[i].cmd(0, NULL);
//            if(ret)
//            {
//                /* LED RED on indicated an error occured! */
//                GPIO_WriteBit(HW_GPIOA, 16, 0);
//                printf("failed!\r\n");
//            }
//            else
//            {
//                printf("ok!\r\n");
//            }  
//        }
//        else
//        {
//            printf("no entry!\r\n");
//        }
//    }
    
    while(1)
    {
        GPIO_PinToggle(LED0_PORT, LED0_PIN);
        GPIO_PinToggle(LED1_PORT, LED1_PIN);
        GPIO_PinToggle(LED2_PORT, LED2_PIN);
        GPIO_PinToggle(LED3_PORT, LED3_PIN);
        printf("!!!\r\n");
        DelayMs(500);
    }
}

void LPUART0_IRQHandler(void)
{
    uint8_t ch;
    LPUART_ReadByte(HW_LPUART0, &ch);
    printf("%c", ch);
}

