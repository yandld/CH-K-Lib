#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "gui.h"
#include "pit.h"
#include "spi.h"
#include "ili9320.h"
#include "ads7843.h"

#include <stdio.h>
#include <stdlib.h>
 
static int Stat1;
static int Stat2;
float DisData;

void LASER_Open(uint32_t instance)
{
    UART_WriteByte(instance, 0x80);
    UART_WriteByte(instance, 0x06);
    UART_WriteByte(instance, 0x05);
    UART_WriteByte(instance, 0x01);
    UART_WriteByte(instance, 0x74);
}

void LASER_Close(uint32_t instance)
{
    UART_WriteByte(instance, 0x80);
    UART_WriteByte(instance, 0x06);
    UART_WriteByte(instance, 0x05);
    UART_WriteByte(instance, 0x00);
    UART_WriteByte(instance, 0x75);
}

void LASER_Single(uint32_t instance)
{
    UART_WriteByte(instance, 0x80);
    UART_WriteByte(instance, 0x06);
    UART_WriteByte(instance, 0x02);
    UART_WriteByte(instance, 0x78);  
}

void LASER_Continue(uint32_t instance)
{
    UART_WriteByte(instance, 0x80);
    UART_WriteByte(instance, 0x06);
    UART_WriteByte(instance, 0x03);
    UART_WriteByte(instance, 0x77);  
}


static void GPIOE_ISR(uint32_t array)
{
    if(array & (1<<26))
    {
        DelayMs(10);
        if(GPIO_ReadBit(HW_GPIOE, 26) == 0)
        {
            Stat1 ^= 1;
            if(Stat1 & 1)
            {
                LASER_Open(HW_UART5);
                printf("1 ON\r\n");
            }
            else
            {
                LASER_Close(HW_UART5);
                printf("1 OFF\r\n");
            }
        }
    }
    if(array & (1<<27))
    {
        DelayMs(10);
        if(GPIO_ReadBit(HW_GPIOE, 27) == 0)
        {
            Stat2 ^= 1;
            if(Stat2 & 1)
            {
                LASER_Continue(5);
                printf("2 ON\r\n");
            }
            else
            {
                LASER_Close(HW_UART5);
                printf("2 OFF\r\n");
            }
        }
    }
}

static void GPIOA_ISR(uint32_t array)
{
    if(array & (1<<4))
    {
        DelayMs(10);
        if(GPIO_ReadBit(HW_GPIOA, 4) == 0)
        {
            printf("TRIG!\r\n");
            LASER_Single(HW_UART5);
            Stat1 = 0;
            Stat2 = 0;
        }
    }
}

enum
{
    kIdle,
    kSOF,
    kCMD ,
    kSINGLE,
    kCHKSUM,
};

void RecvProc(uint8_t data)
{
    static uint8_t buf[32];
    static int cnt;
    static uint32_t stat = kIdle;
    switch(stat)
    {
        case kIdle:
            if(data == 0x80)
            {
                stat = kSOF;
            }
            break;
        case kSOF:
            if(data == 0x06)
            {
                stat = kCMD;
            }
            else
            {
                stat = kIdle;
            }
            break;
        case kCMD:
            if((data == 0x82) || (data == 0x83))
            {
                stat = kSINGLE;
                cnt = 0;
            }
            else
            {
                stat = kIdle;
            }
            break;
        case kSINGLE:
            buf[cnt++] = data;
            if(cnt >=7)
            {
                stat= kCHKSUM;
            }
            break;
        case kCHKSUM:
            DisData = atof(buf);
            printf("%s\r\n", buf);
            stat = kIdle;
            break;
    }
}

void UART5_ISR(uint16_t data)
{
    RecvProc(data);
//    printf("%c", data);
}

int main(void)
{
    int i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    UART_QuickInit(UART5_RX_PD08_TX_PD09, 9600);
    UART_CallbackRxInstall(HW_UART5, UART5_ISR);
    UART_ITDMAConfig(HW_UART5, kUART_IT_Rx, true);
    
    printf("PS!\r\n");
    
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOE, 27, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOA, 4,  kGPIO_Mode_IPU);

    
    GUI_Init();
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_32B_1);
    
    GPIO_CallbackInstall(HW_GPIOE, GPIOE_ISR);
    GPIO_CallbackInstall(HW_GPIOA, GPIOA_ISR);

    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(HW_GPIOE, 27, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(HW_GPIOA, 4,  kGPIO_IT_FallingEdge, true);
    
    while(1)
    {
        GPIO_ReadBit(HW_GPIOE, 26);
        
        if(Stat1 & 0x01)
        {
            GUI_DispStringAt("Laser ON  ", 0, 10);
        }
        else
        {
            GUI_DispStringAt("Laser OFF  ", 0, 10);
        }
        
        if(Stat2 & 0x01)
        {
            GUI_DispStringAt("Cont_M ON  ", 0, 50);
        }
        else
        {
            GUI_DispStringAt("Cont_M OFF ", 0, 50);
        }
        
        uint8_t buf[32];
        sprintf(buf, "Dis:%2.3f   \r\n", DisData);
        GUI_DispStringAt(buf, 0, 100);
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
}


