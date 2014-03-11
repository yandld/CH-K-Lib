#include "ov7620.h"
#include "gpio.h"
#include "board.h"

static uint32_t gHREF_RecievedCnt;

static void OV7620_ISR(uint32_t pinArray)
{
    if(pinArray & (1<<BOARD_OV7620_VSYNC_PIN))
    {
        printf("%d\r\n", gHREF_RecievedCnt);
        gHREF_RecievedCnt = 0;
    }
    if(pinArray & (1<<BOARD_OV7620_HREF_PIN))
    {
        gHREF_RecievedCnt++;
    }
    
}

void OV7620_Init()
{
    uint8_t i;
    //³õÊ¼»¯
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(BOARD_OV7620_DATA_PORT, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV7620_ISR); 
    GPIO_CallbackInstall(BOARD_OV7620_HREF_PORT, OV7620_ISR); 
    GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_RisingEdge);
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_RisingEdge);

}














