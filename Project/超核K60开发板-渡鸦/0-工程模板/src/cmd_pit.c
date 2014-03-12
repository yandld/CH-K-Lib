#include "shell.h"
#include "clock.h"
#include "pit.h"


static void PIT0_CallBack(void)
{
    shell_printf("Enter PIT0 INt\r\n");
}

static void PIT1_CallBack(void)
{
    shell_printf("Enter PIT1 INt\r\n");
}

static void PIT2_CallBack(void)
{
    shell_printf("Enter PIT2 INt\r\n");
}

static void PIT3_CallBack(void)
{
    shell_printf("Enter PIT3 INt\r\n");
}

#define CMD_PIT0_TIME_US  (1000*500)
#define CMD_PIT1_TIME_US  (1000*1000)
#define CMD_PIT2_TIME_US  (1000*2000)
#define CMD_PIT3_TIME_US  (1000*4000)
static const uint32_t CMD_PIT_IntervalTable[] = 
{
    1000*500,
    1000*1000,
    1000*1500,
    1000*2000,
};

int CMD_PIT(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("PIT Test CMD\r\n");
    for(i = 0; i < ARRAY_SIZE(CMD_PIT_IntervalTable); i++)
    {
        shell_printf("PIT%d TIME:%dMS\r\n", i, CMD_PIT_IntervalTable[i]/1000);
    }
    PIT_QuickInit(HW_PIT0_CH0, CMD_PIT0_TIME_US);
    PIT_CallbackInstall(HW_PIT0_CH0, PIT0_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH0, kPIT_IT_TOF);
    
    PIT_QuickInit(HW_PIT0_CH1, CMD_PIT1_TIME_US);
    PIT_CallbackInstall(HW_PIT0_CH1, PIT1_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH1, kPIT_IT_TOF);
    
    PIT_QuickInit(HW_PIT0_CH2, CMD_PIT2_TIME_US);
    PIT_CallbackInstall(HW_PIT0_CH2, PIT2_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH2, kPIT_IT_TOF);
    
    PIT_QuickInit(HW_PIT0_CH3, CMD_PIT3_TIME_US);
    PIT_CallbackInstall(HW_PIT0_CH3, PIT3_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH3, kPIT_IT_TOF);
    return 0;
}




const cmd_tbl_t CommandFun_PIT = 
{
    .name = "PIT",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_PIT,
    .usage = "PIT TEST",
    .complete = NULL,
    .help = "\r\n"

};
