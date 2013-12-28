#include "shell.h"
#include "gpio.h"

char * const CMD_GPIOFunctionSelectTable[] = 
{
    "Init",
    "WriteBit",
    "ReadBit",
    "ToggleBit",
    "WriteData",
    "ReadData",
};


char * const CMD_GPIOModeSelectTable[] = 
{
    "kGPIO_Mode_IFT",
    "kGPIO_Mode_IPD",
    "kGPIO_Mode_IPU",
    "kGPIO_Mode_OOD",
    "kGPIO_Mode_OPP",
};

int DoGPIO(int argc, char *const argv[])
{
    uint8_t i;
    uint32_t gpio_instance;
    uint32_t gpio_pinx;
    uint8_t gpio_mode;
    if(argc == 5 && (!strcmp(argv[1], "Init")))
    {
        switch(argv[2][0])
        {
            case 'A':
                gpio_instance = HW_GPIOA;
                break;
            case 'B':
                gpio_instance = HW_GPIOB;
                break;
            case 'C':
                gpio_instance = HW_GPIOC;
                break;
            case 'D':
                gpio_instance = HW_GPIOD;
                break;
            case 'E':
                gpio_instance = HW_GPIOE;
                break;
        }
        gpio_pinx = strtoul(argv[3], 0, 0);
        i = ARRAY_SIZE(CMD_GPIOModeSelectTable);
        while(i--)
        {
            if(!strcmp(argv[4], CMD_GPIOModeSelectTable[i]))
            {
                gpio_mode = i;
            }
        }
        shell_printf("%d %d %d \r\n", gpio_instance, gpio_pinx, gpio_mode);
        GPIO_QuickInit(gpio_instance, gpio_pinx, (GPIO_Mode_Type)gpio_mode);
    }
    return 0;
}



int DoGPIOComplete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
    uint8_t str_len;
    uint8_t found = 0;
    uint8_t i;
    str_len = strlen(argv[argc-1]);
    switch(argc)
    {
        case 2:
            i = ARRAY_SIZE(CMD_GPIOFunctionSelectTable);
            while(i--)
            {
                if(!strncmp(argv[argc-1], CMD_GPIOFunctionSelectTable[i], str_len))
                {
                    cmdv[found] = CMD_GPIOFunctionSelectTable[i];
                    found++;
                }
            }
            break;

        case 5:
            i = ARRAY_SIZE(CMD_GPIOModeSelectTable);
            while(i--)
            {
                if(!strncmp(argv[argc-1], CMD_GPIOModeSelectTable[i], str_len))
                {
                    cmdv[found] = CMD_GPIOModeSelectTable[i];
                    found++;
                }
            }
            break;
        default:
            break;
    }
    return found;
}

const cmd_tbl_t CommandFun_GPIO = 
{
    .name = "GPIO",
    .maxargs = 5,
    .repeatable = 1,
    .cmd = DoGPIO,
    .usage = "GPIO",
    .complete = DoGPIOComplete,
    .help = "\r\n"
                "GPIO clock - print CPU clock\r\n"
                "GPIO memory   - print CPU memory info"
};

