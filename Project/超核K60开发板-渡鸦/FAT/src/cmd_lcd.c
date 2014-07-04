
#include "shell.h"
#include "ili9320.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include "board.h"       


int CMD_LCD(int argc, char * const * argv)
{
    uint32_t i;
    ili9320_init();
    ili9320_clear(RED);
    return 0;
}


SHELL_EXPORT_CMD(CMD_LCD, LCD, LCD TEST);

