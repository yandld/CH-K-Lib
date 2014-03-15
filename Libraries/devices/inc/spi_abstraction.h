/**
  ******************************************************************************
  * @file    spi_abstraction.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __SPI_ABSTRACTION_H__
#define __SPI_ABSTRACTION_H__

#include <stdint.h>

//!< SPI frame format
typedef enum 
{
    kSPI_ABS_CPOL0_CPHA0,
    kSPI_ABS_CPOL0_CPHA1,
    kSPI_ABS_CPOL1_CPHA0,
    kSPI_ABS_CPOL1_CPHA1
}SPI_ABS_FrameFormat_Type;

//!< SPI return status
typedef enum
{
    kSPI_ABS_StatusOK,
    kSPI_ABS_StatusError,
    kSPI_ABS_StatusUnsupported,
}SPI_ABS_Status;

//!< spi bit order
typedef enum
{
    kSPI_MsbFirst,
    kSPI_LSBFirst,
}SPI_ABS_BitOrder_Type;


//!< API functions
int SPI_ABS_Init(int spiMode, int bitOrder, int baudrate);


#endif

