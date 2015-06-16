#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "spi.h"
#include "common.h"
#include "dma.h"
#include "lpuart.h"
#include "i2c.h"


#include "w25qxx.h"


struct cmd_t
{
    char		*name;
    uint8_t		maxargs;
    int         (*cmd)(int argc, char * const argv[]);
    char		*help;
};
    



int cmd_gpio(int argc, char * const argv[]);
int cmd_dma(int argc, char * const argv[]);
int cmd_i2c(int argc, char * const argv[]);
int cmd_spi(int argc, char * const argv[]);
int cmd_adc(int argc, char * const argv[]);
int cmd_usb(int argc, char * const argv[]);

static struct cmd_t cmd[]=
{
    "LED", 5, cmd_gpio,     "do led test",
    "I2C", 5, cmd_i2c,      "do i2c test",
    "DMA", 5, cmd_dma,      "do dma test", 
    "ADC", 5, cmd_adc,      "do adc test",
    "USB", 5, cmd_usb,      "do usb test",
    "SPI", 5, cmd_spi,      "do spi test", 
   "LOOP", 5, NULL,         "do loop test",
};

int main(void)
{
    int i, ret;
    DelayInit();

    /* UART */
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    LPUART_ITDMAConfig(HW_LPUART0, kUART_IT_Rx, true);
    printf("HelloWorld!\r\n");

    /* do test */
    for(i=0; i<ARRAY_SIZE(cmd); i++)
    {
        printf("processing %s cmd...\r\n", cmd[i].name);
        if(cmd[i].cmd)
        {
            ret = cmd[i].cmd(0, NULL);
            if(ret)
            {
                /* LED RED on indicated an error occured! */
                GPIO_WriteBit(HW_GPIOA, 16, 0);
                printf("failed!\r\n");
            }
            else
            {
                printf("ok!\r\n");
            }  
        }
        else
        {
            printf("no entry!\r\n");
        }
    }
    
    while(1)
    {

    }
}

void LPUART0_IRQHandler(void)
{
    uint8_t ch;
    LPUART_ReadByte(HW_LPUART0, &ch);
    printf("%c", ch);
}

