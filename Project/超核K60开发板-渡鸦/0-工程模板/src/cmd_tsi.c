#include "shell.h"
#include "common.h"
#include "systick.h"
#include "tsi.h"
#include "gpio.h"



void TSI_ISR(uint32_t outOfRangeArray)
{
    static uint32_t last_chl_array;
    uint32_t value;
    TSI_ITDMAConfig(kTSI_IT_Disable);
    value = outOfRangeArray^last_chl_array;
    if(value & outOfRangeArray & (1<<1))
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
    }
    if(value & outOfRangeArray & (1<<2))
    {
        GPIO_ToggleBit(HW_GPIOE, 12);
    }
    if(value & outOfRangeArray & (1<<3))
    {
        GPIO_ToggleBit(HW_GPIOE, 11);
    }
    if(value & outOfRangeArray & (1<<4))
    {
        GPIO_ToggleBit(HW_GPIOE, 7);
    }
    last_chl_array = outOfRangeArray;
    TSI_ITDMAConfig(kTSI_IT_EndOfScan);
}

int CMD_TSI(int argc, char * const argv[])
{
    printf("TSI TEST\r\n");
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP);
    TSI_QuickInit(TSI0_CH1_PA00);
    TSI_QuickInit(TSI0_CH2_PA01);
    TSI_QuickInit(TSI0_CH3_PA02);
    TSI_QuickInit(TSI0_CH4_PA03);
    TSI_CallbackInstall(TSI_ISR);
    TSI_ITDMAConfig(kTSI_IT_EndOfScan);
    while(1)
    {

    }
}



const cmd_tbl_t CommandFun_TSI = 
{
    .name = "TSI",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_TSI,
    .usage = "TSI",
    .complete = NULL,
    .help = "TSI"
};
