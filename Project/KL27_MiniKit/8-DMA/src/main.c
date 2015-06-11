#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "common.h"
#include "lpuart.h"
#include "dma.h"


int cmd_dma(int argc, char * const argv[])
{
    int i, ret;
    uint8_t src_buf[16];
    uint8_t dest_buf[16];

    ret = 0;
    for(i=0;i<sizeof(src_buf);i++)
    {
        src_buf[i] = i;
        dest_buf[i] = 0;
    }
    
    DMA_InitTypeDef Init;

    Init.chl = HW_DMA_CH0;
    Init.chlTriggerSource = MUX0_DMAREQ;
    Init.triggerSourceMode = kDMA_TriggerSource_Normal;
    Init.transferByteCnt = sizeof(src_buf);

    Init.sAddr = (uint32_t)src_buf;
    Init.sAddrIsInc = true;
    Init.sDataWidth = kDMA_DataWidthBit_8;
    Init.sMod = kDMA_ModuloDisable;

    Init.dAddr = (uint32_t)dest_buf;
    Init.dAddrIsInc = true;
    Init.dDataWidth = kDMA_DataWidthBit_8;
    Init.dMod = kDMA_ModuloDisable;
    DMA_Init(&Init);
    
    DMA_EnableRequest(HW_DMA_CH0);
    while(DMA_GetTransferByteCnt(HW_DMA_CH0) != 0);

    for(i=0;i<sizeof(src_buf);i++)
    {
        if(dest_buf[i] != src_buf[i])
        {
            ret++;
        }
    }
    return ret;
}



int main(void)
{
    int ret;
    
    DelayInit();
    GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
    LPUART_QuickInit(LPUART0_RX_D06_TX_D07, 115200);
    printf("8-DMA demo\r\n");
    ret = cmd_dma(0, NULL);
    
    if(ret)
    {
        printf("DMA test failed\r\n");
    }
    else
    {
        printf("DMA test ok!\r\n");
    }

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOC, 3);
        DelayMs(500);
    }
}

