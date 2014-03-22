#include "shell.h"
#include "gpio.h"
#include "common.h"
#include "board.h"


#include "ftm.h"
#include "shell.h"
#include "gpio.h"
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
    QuickInit_Type QuickInitStruct1;
    req = strtoul(argv[1], 0, 0);
    instance = FTM_PWM_QuickInit(BOARD_BUZZER_FTM_MAP ,req);
    QuickInitDecode(BOARD_BUZZER_FTM_MAP, &QuickInitStruct1);
    FTM_PWM_ChangeDuty(instance, QuickInitStruct1.channel, 5000); 
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
