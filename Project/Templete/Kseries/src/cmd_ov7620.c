#include "shell.h"
#include "gpio.h"
#include "common.h"
#include "board.h"
#include "ov7620.h"
#include "ili9320.h"

#include "ftm.h"
#include "shell.h"
#include "gpio.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
//RGB »ìÉ«

uint16_t RGB2COLOR(uint8_t RR,uint8_t GG,uint8_t BB)
{
  return (((RR/8)<<11)+((GG/8)<<6)+BB/8); 
}


static void OV7620_ISR(char ** image)
{
    uint32_t i,j;
    for(i=0;i<240;i++)
    {
        for(j=0;j<240;j++)
        {
            LCD_DrawPoint(j, 240-i, RGB2COLOR(CCDBuffer[i][j], CCDBuffer[i][j], CCDBuffer[i][j]));
        }
    }
}

int CMD_BOV7620(int argc, char * const * argv)
{
    printf("OV7620 TEST\r\n");
    ili9320_Init();
    OV7620_CallbackInstall(OV7620_ISR);
    OV7620_Init();
    return 0;
}

const cmd_tbl_t CommandFun_OV7620 = 
{
    .name = "OV7620",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_BOV7620,
    .usage = "OV7620",
    .complete = NULL,
    .help = "OV7620"
};
