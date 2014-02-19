#include "ftm.h"
#include "shell/shell.h"





int CMD_FTM(int argc, char * const * argv)
{
    uint8_t instance;
    uint32_t req;
    FTM_InitTypeDef FTM_InitStruct1;
    FTM_InitStruct1.instance = 0;
    FTM_InitStruct1.frequencyInHZ = 32;
    FTM_InitStruct1.mode = kPWM_EdgeAligned;
    printf("FTM TEST\r\n");
    //Only one param
    if(argc != 2)
    {
        return CMD_RET_USAGE;
    }
    if(argc == 2)
    {
        req = strtoul(argv[1], 0, 0);
        instance = FTM_QuickInit(FTM2_CH1_PB19 ,req);
    }
    FTM_PWM_ChangeDuty(instance, 1, 3000); 
    return 0;
}



const cmd_tbl_t CommandFun_FTM = 
{
    .name = "FTM",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_FTM,
    .usage = "FTM <requerency>",
    .complete = NULL,
    .help = "\r\n"
};
