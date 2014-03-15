/**
  ******************************************************************************
  * @file    spi_abstraction.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
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

typedef enum
{
    kSPI_ABS_CS_ReturnInactive,     //!< 传输完成后CS信号保持片选中状态
    kSPI_ABS_CS_KeepAsserted,       //!< 传输完成后CS信号保持未选中状态
}SPI_ABS_CSStatus_Type;
  

//!< API functions
int SPI_ABS_Init(int frameFormat, int baudrate);
int SPI_ABS_xfer(uint8_t *dataSend, uint8_t *dataReceived, uint32_t cs, uint16_t csState, uint32_t len);





#endif

