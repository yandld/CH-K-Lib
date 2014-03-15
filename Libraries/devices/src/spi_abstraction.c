#include "spi_abstraction.h"
#include "board.h"
#include "spi.h"
#include "clock.h"

static uint32_t gSPIInstance;

 /**
 * @brief  SPI 通用抽象层接口 SPI 初始化
 * 
 * @param  frameFormat: 帧格式
 *         @arg kSPI_ABS_CPOL0_CPHA0:
 *         @arg kSPI_ABS_CPOL0_CPHA1:
 *         @arg kSPI_ABS_CPOL1_CPHA0:
 *         @arg kSPI_ABS_CPOL1_CPHA1:
 * @retval None
 */
int SPI_ABS_Init(int frameFormat, int baudrate)
{
    gSPIInstance = SPI_QuickInit(BOARD_SPI_MAP, frameFormat, baudrate);
    return kSPI_ABS_StatusOK;
}
 /**
 * @brief  SPI 传送一个字节
 * 
 * @param  data: 发送的数据
 * @param  cs:   PCS 使能信号选择
 * @param  csState: 发送数据后CS线的状态
 *         @arg kSPI_ABS_CS_ReturnInactive: 回到未选中状态
 *         @arg kSPI_ABS_CS_KeepAsserted:   继续保持选中状态
 * @retval 接收到的数据
 */
int SPI_ABS_ReadWriteByte(uint8_t data, uint32_t cs, uint16_t csState)
{
    return SPI_ReadWriteByte(gSPIInstance, data, cs, csState);
}

int SPI_ABS_xfer(uint8_t *dataSend, uint8_t *dataReceived, uint32_t cs, uint16_t csState, uint32_t len)
{
    if(csState == kSPI_ABS_CS_KeepAsserted)
    {
        len--;
        while(len--)
        {
            *dataReceived++ = SPI_ReadWriteByte(gSPIInstance, *dataSend++, cs, kSPI_ABS_CS_KeepAsserted);
        }
        *dataReceived++ = SPI_ReadWriteByte(gSPIInstance, *dataSend++, cs, kSPI_ABS_CS_ReturnInactive);
    }
    else if(csState == kSPI_ABS_CS_ReturnInactive)
    {
        while(len--)
        {
            *dataReceived++ = SPI_ReadWriteByte(gSPIInstance, *dataSend++, cs, kSPI_ABS_CS_ReturnInactive);
        }
    }
    return kSPI_ABS_StatusOK;
}



int SPI_ABS_AbortTransfer(void)
{
    return kSPI_ABS_StatusUnsupported;
}

/*
void SPI_ABS_SetCSHigh(void)
{
    
}

void SPI_ABS_SetCSLow(void)
{
    
}
*/







