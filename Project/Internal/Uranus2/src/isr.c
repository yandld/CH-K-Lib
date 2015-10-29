#include <string.h>


#include "common.h"
#include "dma.h"
#include "mq.h"
#include "protocol.h"

#include "appdef.h"





void PIT_IRQHandler(void)
{
    if(PIT->CHANNEL[0].TFLG)
    {
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
        msg_t msg;
        msg.cmd = kMSG_CMD_TIMER;
        mq_push(msg);
    }
}

void UART0_IRQHandler(void)
{
    static uint8_t buf[64];
    uint32_t len;
    static rev_data_t rd;
    if((UART0->S1 & UART_S1_RDRF_MASK) && (UART0->C2 & UART_C2_RIE_MASK))
    {
       // gRevBuf[i++] = UART0->D;
    }
    
    if(UART0->S1 & UART_S1_IDLE_MASK)
    {
        UART0->S1 |=  UART_S1_IDLE_MASK;
        
        len = 9999 - DMA_GetTransCnt(DMA_RX_CH);
        memcpy(buf, gRevBuf, len);
        DMA_SetDestAddr(DMA_RX_CH, (uint32_t)gRevBuf);
        DMA_SetTransCnt(DMA_RX_CH, 9999);
        ano_rec(&rd, buf, len);
    }
    
    /**
        this is very important because in real applications
        OR usually occers, the RDRF will block when OR is asserted 
    */
    if(UART0->S1 & UART_S1_OR_MASK)
    {
      //  ch = UART0->D;
    }
    
}



void PORTA_IRQHandler(void)
{
    PORTA->ISFR |= (1 << MPU9250_INT_PIN);
    msg_t msg;
    msg.cmd = kMSG_CMD_SENSOR_DATA_READY;
    mq_push(msg);
}





