#include "shell.h"
#include "gpio.h"
#include "common.h"
#include "board.h"


#include "ftm.h"
#include "shell.h"
#include "gpio.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>


int CMD_BUZZER(int argc, char * const * argv)
{
    uint8_t instance = 0;
    uint32_t req;

    printf("BUZZER TEST\r\n");
    //quick init
    if(argc !=2)
    {
        return CMD_RET_USAGE;
    }
    
    req = strtoul(argv[1], 0, 0);
    instance = FTM_QuickInit(BOARD_BUZZER_FTM_MAP ,req);
    FTM_PWM_ChangeDuty(instance, FTM_GetChlFormQuickMAP(BOARD_BUZZER_FTM_MAP), 5000); 
    return 0;
}



const cmd_tbl_t CommandFun_BUZZER = 
{
    .name = "BUZZER",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_BUZZER,
    .usage = "BUZZER <req>",
    .complete = NULL,
    .help = "BUZZER <req>"
};
