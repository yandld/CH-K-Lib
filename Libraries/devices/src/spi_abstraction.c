#include "spi_abstraction.h"
#include "board.h"
#include "spi.h"
#include "clock.h"

static uint32_t gSPIInstance;

int SPI_ABS_Init(int spiMode, int bitOrder, int baudrate)
{
    uint32_t clock;
    uint32_t div = 2;
    uint32_t absError = 0xFFFFFFFF;
    uint32_t bestError = 0xffffffffu;
    uint32_t SPIxMAP = BOARD_SPI_MAP;
    SPI_InitTypeDef SPI_InitStruct1;
    QuickInit_Type * pSPIxMap = (QuickInit_Type*)&(SPIxMAP);
    gSPIInstance = pSPIxMap->ip_instance;
    // caluate baud
    CLOCK_GetClockFrequency(kBusClock, &clock);
    clock/=1000; //clock uint to KHz
    while(1)
    {
        if(abs((clock/div) - baudrate) < absError)
        {
            absError = abs((clock/div) - baudrate);
        }
        div<<1;
    }
    SPI_InitStruct1.baudrateDivSelect = kSPI_BaudrateDiv_128;
    SPI_InitStruct1.frameFormat = kSPI_CPOL0_CPHA1;
    SPI_InitStruct1.dataSizeInBit = 8;
    SPI_InitStruct1.instance = gSPIInstance;
    SPI_InitStruct1.mode = kSPI_Master;
    SPI_InitStruct1.bitOrder = kSPI_MSBFirst;
    SPI_Init(&SPI_InitStruct1);
    return kSPI_ABS_StatusOK;
}

int SPI_ABS_ReadWriteByte(void)
{
    
}

int SPI_ABS_AbortTransfer(void)
{
    
}
/*
void SPI_ABS_SetCSHigh(void)
{
    
}

void SPI_ABS_SetCSLow(void)
{
    
}
*/







