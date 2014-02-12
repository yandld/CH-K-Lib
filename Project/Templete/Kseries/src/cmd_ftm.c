#include "ftm.h"
#include "shell/shell.h"









int CMD_FTM(int argc, char * const * argv)
{
    uint8_t instance;
    FTM_InitTypeDef FTM_InitStruct1;
    FTM_InitStruct1.instance = 0;
    FTM_InitStruct1.frequencyInHZ = 320000;
    FTM_InitStruct1.mode = PWM_EdgeAligned;
    
    FTM_Init(&FTM_InitStruct1);
    return 0;
}






const cmd_tbl_t CommandFun_FTM = 
{
    .name = "FTM",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_FTM,
    .usage = "FTM TEST",
    .complete = NULL,
    .help = "\r\n"
};
