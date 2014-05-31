#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "FlashOS.H"

/* 
    SSD 是ARM标准Flash操作库
    MK_P512.lib 是从Keil/ARM/Flash下中提取出来的操作库 适用于MKDN512xx系列 其接口函数请参见FlashOS.H
*/

static int MKP512FlashInit(void)
{
    uint32_t clock;
    uint32_t flash_clock = CLOCK_GetClockFrequency(kFlashClock, &clock);
    /* fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify) */         
    return Init(0x00000000, clock, 2);
}

#define SECTER_SIZE     0x000800
extern int Image$$ER_IROM1$$Limit;
#define TEST_ADDR_BEIGN  (uint32_t)(&Image$$ER_IROM1$$Limit + SECTER_SIZE)

int main(void)
{
    static uint8_t buf[SECTER_SIZE];
    uint32_t i;
    uint32_t r;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("init standard flash driver...\r\n");
    r = MKP512FlashInit();
    if(r)
    {
        printf("flash init failed\r\n");
    }
    printf("flash test start addr:0x%08X size:0x%08X\r\n", TEST_ADDR_BEIGN, sizeof(buf));
    
    /* write test data */
    for(i=0;i<SECTER_SIZE;i++)
    {
        buf[i] = i%0xFF;
    }

    /* ersase and program */
    EraseSector(TEST_ADDR_BEIGN);
    r = ProgramPage(TEST_ADDR_BEIGN, sizeof(buf), buf);
    if(r)
    {
        printf("program page failed\r\n");
    }
    
    /* verify */
    uint8_t *p = (uint8_t*)TEST_ADDR_BEIGN;
    for(i=0;i<sizeof(buf);i++)
    {
        printf("0x%02X ", *p++);
    }
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


