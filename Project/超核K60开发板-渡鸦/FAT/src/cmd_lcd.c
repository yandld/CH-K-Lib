
#include "shell.h"
#include "ili9320.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include "board.h"       


static uint16_t buf[100]; 
int CMD_LCD(int argc, char * const * argv)
{
    uint32_t i;
    ili9320_init();
    memset(buf, BLUE, sizeof(buf));
  //  ili9320_set_window(0,0,10,10);
 //   ili9320_write_gram(buf, ARRAY_SIZE(buf));
    ili9320_hline(0,60,50,RED);
    ili9320_vline(100, 150, 90, GREEN);
}

const cmd_tbl_t CommandFun_LCD = 
{
    .name = "LCD",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_LCD,
    .usage = "LCD",
    .complete = NULL,
    .help = "\r\n"
};
