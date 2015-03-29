#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pit.h"
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

    /* func:Init is SSD API */    
    return Init(0x00000000, clock, 2); 
}

/* chip's Flash size and sector size can be found in RM */
#define FLASH_SIZE      0x80000
#define SECTER_SIZE     0x000800

/* get  residue flash size */
#define TEST_ADDR_BEIGN  (uint32_t)(FLASH_SIZE - SECTER_SIZE)

int main(void)
{
    static uint8_t buf[SECTER_SIZE];
    uint32_t i;
    uint32_t ticks;
    uint32_t r;
    DelayInit();
    
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    PIT_QuickInit(HW_PIT_CH0, 1000*1000);
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
    PIT_ResetCounter(HW_PIT_CH0);
    ticks = PIT_GetCounterValue(HW_PIT_CH0);
    EraseSector(TEST_ADDR_BEIGN);
    ticks = ticks - PIT_GetCounterValue(HW_PIT_CH0);
    printf("EraseSector takes %d ticks\r\n", ticks);
    
    /* when program flash, we must erase it first */
    PIT_ResetCounter(HW_PIT_CH0);
    ticks = PIT_GetCounterValue(HW_PIT_CH0);
    r = ProgramPage(TEST_ADDR_BEIGN, sizeof(buf), buf);
    ticks = ticks - PIT_GetCounterValue(HW_PIT_CH0);
    printf("ProgramPage takes %d ticks\r\n", ticks);
    
    if(r)
    {
        printf("program page failed\r\n");
    }
    
    /* verify */
    uint8_t *p = (uint8_t*)TEST_ADDR_BEIGN;
    for(i=0;i<sizeof(buf);i++)
    {
        if(*p++ != (i%0xFF))
        {
            printf("error:0x%02X ", *p++);
        }
    }
    printf("flash test completed!\r\n");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


