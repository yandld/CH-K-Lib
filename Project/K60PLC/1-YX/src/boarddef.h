#ifndef __CH_BOARDDEF_H__
#define __CH_BOARDDEF_H__

#include "chlib_k.h"

#define     BOARD_UART_INSTANCE     HW_UART0
#define     BOARD_UART_MAP          UART0_RX_PD06_TX_PD07

#define     LED0_PORT   HW_GPIOE
#define     LED0_PIN    10
#define     LED1_PORT   HW_GPIOE
#define     LED1_PIN    11
#define     LED2_PORT   HW_GPIOE
#define     LED2_PIN    12
#define     LED3_PORT   HW_GPIOE
#define     LED3_PIN    24

#define     LED0  PEout(LED0_PIN)
#define     LED1  PEout(LED1_PIN)
#define     LED2  PEout(LED2_PIN)
#define     LED3  PEout(LED3_PIN)


#define ADC_CHANNELS        {6, 7, 8, 9,10,11,12,13,10,11,12,13,14,15,21,22};
#define ADC_PORTS           {1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};


#define DIN_PINS            
#define DIN_PORTS           

#define DOUT_PINS           
#define DOUT_PORTS          


#define RS485_CH0_MAP       
#define RS485_CH1_MAP       
#define RS485_CH2_MAP       





#endif

