/**
  ******************************************************************************
  * @file    spi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "spi.h"
#include "gpio.h"
#include "common.h"
#include "clock.h"



#if (!defined(SPI_BASES))

    #if     (defined(MK60DZ10))
    #define UART_BASES {SPI0, SPI1, SPI2}
    #elif   (defined(MK10D5))
    #define UART_BASES {SPI0}
    #endif

#endif
    
    

