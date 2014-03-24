#include "shell.h"
#include "common.h"
#include "systick.h"
#include "tsi.h"


int CMD_TSI(int argc, char * const argv[])
{
    printf("TSI TEST\r\n");
    TSI_InitTypeDef TSI_InitStruct1;
    TSI_Init(&TSI_InitStruct1);
    TSI_QuickInit(TSI0_CH6_PB01);
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
