#include "common.h"
#include "shell.h"


int CMD_NVIC(int argc, char * const * argv)
{
    printf("NVIC Test\r\n");
    uint32_t PreemptPriority, SubPriority;
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
    printf("SystemPrioirityGroup:%d\r\n", NVIC_GetPriorityGrouping());
    
    NVIC_SetPriority(PORTA_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 1));
    NVIC_DecodePriority(NVIC_GetPriority(PORTA_IRQn), NVIC_GetPriorityGrouping(), &PreemptPriority, &SubPriority);
    printf("IRQn:%d PreemptPriority:%d SubPriority:%d\r\n", (uint32_t)PORTA_IRQn, PreemptPriority, SubPriority);
    
    NVIC_SetPriority(PORTB_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2));
    NVIC_DecodePriority(NVIC_GetPriority(PORTB_IRQn), NVIC_GetPriorityGrouping(), &PreemptPriority, &SubPriority);
    printf("IRQn:%d PreemptPriority:%d SubPriority:%d\r\n", (uint32_t)PORTA_IRQn, PreemptPriority, SubPriority);

    return 0;
}

const cmd_tbl_t CommandFun_NVIC = 
{
    .name = "NVIC",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_NVIC,
    .usage = "NVIC",
    .complete = NULL,
    .help = "\r\n"
};
