#include "shell.h"
#include "gpio.h"
#include "common.h"
#include "board.h"

int DoGPIO(int argc, char *const argv[])
{
    uint32_t i,j;
    uint32_t LED_GPIOInstanceTable[] = BOARD_LED_GPIO_BASES;
    uint32_t led_num = ARRAY_SIZE(LED_GPIOInstanceTable);
    uint32_t LED_PinTable[] = BOARD_LED_PIN_BASES;
    
    for(i = 0; i < led_num; i++)
    {
        GPIO_QuickInit(LED_GPIOInstanceTable[i], LED_PinTable[i], kGPIO_Mode_OPP);  
    }
    for(i = 0; i < 20; i++)
    {
        for(j = 0; j < led_num; j++)
        {
            GPIO_ToggleBit(LED_GPIOInstanceTable[j], LED_PinTable[j]);
        }
        DelayMs(40);
    }
    return 0;
}



const cmd_tbl_t CommandFun_GPIO = 
{
    .name = "GPIO",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoGPIO,
    .usage = "GPIO",
    .complete = NULL,
    .help = "\r\n"
                "GPIO clock - print CPU clock\r\n"
                "GPIO memory   - print CPU memory info"
};

