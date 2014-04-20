#include "gpio.h"
#include "uart.h"
#include "24l01.h"
#include "ili9320.h"
#include "spi.h"

static struct spi_bus bus;
extern int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance);
static uint8_t NRF2401RXBuffer[32] = "HelloWorld\r\n";//无线接收数据
static uint8_t* gpRevChar;

/* 串口接收中断 */
void UART_ISR(uint16_t ch)
{
    static uint8_t rev_ch;
    rev_ch = ch;
    gpRevChar = (uint8_t*)&rev_ch;
}

int main(void)
{
    uint32_t i;
    uint32_t ret;
    uint32_t len;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_CallbackRxInstall(HW_UART0, UART_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx);
    
    printf("NRF24L01 test\r\n");
    /* 初始化 SPI接口及片选 */
    PORT_PinMuxConfig(HW_GPIOE, 1, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 2, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 3, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 4, kPinAlt2);
    /* 初始化2401所需的CE引脚 */
    GPIO_QuickInit(HW_GPIOE, 0 , kGPIO_Mode_OPP);
    /* 初始化2401 */
    static struct spi_bus bus;
    ret = kinetis_spi_bus_init(&bus, HW_SPI1);
    nrf24l01_init(&bus, 0);
    if(nrf24l01_probe())
    {
        printf("no nrf24l01 device found!\r\n");
    }
    /* 进入Rx模式 */
    nrf24l01_set_rx_mode();
    while(1)
    {
        /* 如果收到串口数据则发送 */
        if(gpRevChar != NULL)
        {
            nrf24l01_set_tx_mode();
            nrf24l01_write_packet(gpRevChar, 1);
            nrf24l01_set_rx_mode();
            gpRevChar = NULL;
        }
        /* 如果收到2401 的数据 则传输到串口 */
        if(!nrf24l01_read_packet(NRF2401RXBuffer, &len))
        {
            i = 0;
            while(len--)
            {
                UART_WriteByte(HW_UART0, NRF2401RXBuffer[i++]);
            }
        }
    }
}


