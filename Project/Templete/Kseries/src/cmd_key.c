#include "shell.h"
#include "gpio.h"
#include "common.h"
#include "board.h"


void GPIO_ISR(uint32_t array)
{
    shell_printf("GPIO INT:0x%x\r\n", array);
    
}

int DoKEY(int argc, char *const argv[])
{
    uint32_t i;
    uint32_t KEY_GPIOInstanceTable[] = BOARD_KEY_GPIO_BASES;
    uint32_t key_num = ARRAY_SIZE(KEY_GPIOInstanceTable);
    uint32_t KEY_PinTable[] = BOARD_KEY_PIN_BASES;
    
    for(i = 0; i < key_num; i++)
    {
        GPIO_QuickInit(KEY_GPIOInstanceTable[i], KEY_PinTable[i], kGPIO_Mode_IPU);  
        GPIO_CallbackInstall(KEY_GPIOInstanceTable[i], GPIO_ISR);
        GPIO_ITDMAConfig(KEY_GPIOInstanceTable[i], KEY_PinTable[i], kGPIO_IT_RisingEdge);
    }        
    return 0;
}



const cmd_tbl_t CommandFun_KEY = 
{
    .name = "KEY",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoKEY,
    .usage = "KEY",
    .complete = NULL,
    .help = "\r\n"

};

