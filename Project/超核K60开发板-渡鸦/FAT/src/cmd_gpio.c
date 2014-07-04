#include "shell.h"
#include "gpio.h"
#include "systick.h"
#include "board.h"

static int CMD_GPIO_LED(int argc, char *const argv[])
{
    uint32_t i,j;
    uint32_t LED_GPIOInstanceTable[] = BOARD_LED_GPIO_BASES;
    uint32_t led_num = ARRAY_SIZE(LED_GPIOInstanceTable);
    uint32_t LED_PinTable[] = BOARD_LED_PIN_BASES;
    for(i = 0; i < led_num; i++)
    {
        GPIO_QuickInit(LED_GPIOInstanceTable[i], LED_PinTable[i], kGPIO_Mode_OPP);  
    }
    for(i = 0; i < 10; i++)
    {
        for(j = 0; j < led_num; j++)
        {
            GPIO_ToggleBit(LED_GPIOInstanceTable[j], LED_PinTable[j]);
        }
        DelayMs(100);
    }
    return 0;
}

void GPIO_ISR(uint32_t array)
{
    shell_printf("GPIO INT:0x%x\r\n", array);
}

static int CMD_GPIO_KEY(int argc, char *const argv[])
{
    uint32_t i;
    uint32_t KEY_GPIOInstanceTable[] = BOARD_KEY_GPIO_BASES;
    uint32_t key_num = ARRAY_SIZE(KEY_GPIOInstanceTable);
    uint32_t KEY_PinTable[] = BOARD_KEY_PIN_BASES;
    printf(" PRESS ANY KEY ...\r\n");
    for(i = 0; i < key_num; i++)
    {
        GPIO_QuickInit(KEY_GPIOInstanceTable[i], KEY_PinTable[i], kGPIO_Mode_IPU);  
        GPIO_CallbackInstall(KEY_GPIOInstanceTable[i], GPIO_ISR);
        GPIO_ITDMAConfig(KEY_GPIOInstanceTable[i], KEY_PinTable[i], kGPIO_IT_RisingEdge, true);
        PORT_PinPassiveFilterConfig(KEY_GPIOInstanceTable[i], KEY_PinTable[i], ENABLE);
    }
    return 0;
}

static int CMD_GPIO_BUZZER(int argc, char *const argv[])
{
    uint32_t i;

    GPIO_QuickInit(BOARD_BUZZER_GPIO_PORT, BOARD_BUZZER_GPIO_PIN, kGPIO_Mode_OPP);  
    while(1)
    {
        GPIO_ToggleBit(BOARD_BUZZER_GPIO_PORT, BOARD_BUZZER_GPIO_PIN);
        DelayMs(20);
    }
    return 0;
}

static int CMD_GPIO_CTRL(int argc, char *const argv[])
{
    
    return 0;
}

int CMD_GPIO(int argc, char *const argv[])
{
    if((argc == 2) && (!strcmp(argv[1], "LED")))
    {
        return CMD_GPIO_LED(0, NULL);
    }
    if((argc == 2) && (!strcmp(argv[1], "KEY")))
    {
        return CMD_GPIO_KEY(0, NULL);
    }
    if((argc == 2) && (!strcmp(argv[1], "BUZZER")))
    {
        return CMD_GPIO_BUZZER(0, NULL);
    }
    if(argc == 3)
    {
        return CMD_GPIO_CTRL(argc, argv);
    }
    return CMD_RET_USAGE;
}

SHELL_EXPORT_CMD(CMD_GPIO, GPIO, GPIO <CMD> (CMD = LED, KEY, BUZZER));

