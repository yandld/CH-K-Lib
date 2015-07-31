#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "spi.h"
#include "dma.h"
#include "w25qxx.h"


#define DMA_SPI_TX_CH   (0)
#define DMA_SPI_RX_CH   (1)

static uint32_t gProbleCmd[] = 
{
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0x90),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (1 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
    (0 << SPI_PUSHR_CONT_SHIFT) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1) | SPI_PUSHR_TXDATA(0xFF),
};

uint32_t rev_buf[32];

void DMA1_IRQHandler(void)
{
    int i;
    DMA0->CINT = DMA_CINT_CINT(DMA_SPI_RX_CH);
    printf("spi flash id:0x%X\r\n", (rev_buf[5]<<8) + rev_buf[4]);
}

void SPI_xfer(uint32_t *buf_in, uint32_t *buf_out, uint32_t size)
{
    int i;
    
    /* format the send buffer */
    for(i=0; i<size; i++)
    {
        buf_in[i] |= SPI_PUSHR_CONT_MASK | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1<<1);
    }
    buf_in[size-1] &= ~SPI_PUSHR_CONT_MASK;
    
    /* set SPI Tx DMA ch's source addr */
    DMA0->TCD[DMA_SPI_TX_CH].SADDR = (uint32_t)buf_in;
    /* set SPI Rx DMA ch's dest addr */
    DMA0->TCD[DMA_SPI_RX_CH].DADDR = (uint32_t)buf_out;

    /* set major loop */
    DMA0->TCD[DMA_SPI_TX_CH].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(size);
    DMA0->TCD[DMA_SPI_RX_CH].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(size);
    
    /* enable request */
    DMA0->ERQ = (1 << DMA_SPI_TX_CH) | (1 << DMA_SPI_RX_CH);

}

static void UART_SPI_Config(void)
{
    /* enable DMA and DMAMUX clock */
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

    /* configure DMAMUX */
    DMAMUX->CHCFG[DMA_SPI_TX_CH]= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(SPI2_TRAN_DMAREQ);
    DMAMUX->CHCFG[DMA_SPI_RX_CH]= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(SPI2_REV_DMAREQ);
    
    
    /* source configuration MOSI */
    DMA0->TCD[DMA_SPI_TX_CH].SADDR = NULL;
    DMA0->TCD[DMA_SPI_TX_CH].ATTR |= DMA_ATTR_SSIZE(2);
    DMA0->TCD[DMA_SPI_TX_CH].SOFF = 4;
    DMA0->TCD[DMA_SPI_TX_CH].SLAST = 0;
    
    /* destination configuration MOSI */
    DMA0->TCD[DMA_SPI_TX_CH].DADDR = (uint32_t)&SPI2->PUSHR;
    DMA0->TCD[DMA_SPI_TX_CH].ATTR |= DMA_ATTR_DSIZE(2);
    DMA0->TCD[DMA_SPI_TX_CH].DOFF= 0;
    DMA0->TCD[DMA_SPI_TX_CH].DLAST_SGA= 0;
    
    /* set CITER and BITER to maximum value MOSI */
    DMA0->TCD[DMA_SPI_TX_CH].NBYTES_MLNO = 4;
    
    /* source configuration MISO */
    DMA0->TCD[DMA_SPI_RX_CH].SADDR = (uint32_t)&SPI2->POPR;
    DMA0->TCD[DMA_SPI_RX_CH].ATTR |= DMA_ATTR_SSIZE(2);
    DMA0->TCD[DMA_SPI_RX_CH].SOFF = 0;
    DMA0->TCD[DMA_SPI_RX_CH].SLAST = 0;
    
    /* destination configuration MISO */
    DMA0->TCD[DMA_SPI_RX_CH].DADDR = NULL;
    DMA0->TCD[DMA_SPI_RX_CH].ATTR |= DMA_ATTR_DSIZE(2);
    DMA0->TCD[DMA_SPI_RX_CH].DOFF= 4;
    DMA0->TCD[DMA_SPI_RX_CH].DLAST_SGA= 0;
    
    /* set CITER and BITER to maximum value MISO */
    DMA0->TCD[DMA_SPI_RX_CH].NBYTES_MLNO = 4;
    
    /* enable interrupt enable */
    DMA0->TCD[DMA_SPI_RX_CH].CSR |= DMA_CSR_INTMAJOR_MASK;
    NVIC_EnableIRQ(DMA1_IRQn);
}


int main(void)
{
    int i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("DMA UART transmit test\r\n");

    SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14, kSPI_CPOL0_CPHA0, 1*1000*1000);
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); /* SPI2_PCS1 */
    SPI_EnableTxFIFO(HW_SPI2, true);
    SPI_EnableRxFIFO(HW_SPI2, true);
    
    UART_SPI_Config();
    SPI_ITDMAConfig(HW_SPI2, kSPI_DMA_TFFF, true);
    SPI_ITDMAConfig(HW_SPI2, kSPI_DMA_RFDF, true);

    SPI_xfer(gProbleCmd, rev_buf, 6);
    
    while(1)
    {
        
    }

}


