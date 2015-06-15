#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"

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
    DelayInit();
    /* LED */
    GPIO_QuickInit(LED0_PORT, LED0_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED1_PORT, LED1_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED2_PORT, LED2_PIN, kGPIO_Mode_OPP);
   // GPIO_QuickInit(LED3_PORT, LED3_PIN, kGPIO_Mode_OPP);
    /* KEY k1 */
    GPIO_QuickInit(HW_GPIOB,0, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB,1, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOA,4, kGPIO_Mode_IPU);
    //GPIO_QuickInit(HW_GPIOA,20,kGPIO_Mode_IPU);
 
    /* UART */
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    while(1)
    {   if(GPIO_ReadBit(HW_GPIOB, 0)==0)
              GPIO_WriteBit(LED0_PORT, LED0_PIN, 0);
        else
              GPIO_WriteBit(LED0_PORT, LED0_PIN, 1);
        if(GPIO_ReadBit(HW_GPIOB, 1)==0)
              GPIO_WriteBit(LED2_PORT, LED2_PIN, 0);
        else
              GPIO_WriteBit(LED2_PORT, LED2_PIN, 1);
        
        if(GPIO_ReadBit(HW_GPIOA, 4)==0)
              GPIO_WriteBit(LED1_PORT, LED1_PIN, 0);
        else
              GPIO_WriteBit(LED1_PORT, LED1_PIN, 1);
     }
}


