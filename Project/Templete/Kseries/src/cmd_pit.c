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


int CMD_PIT(int argc, char * const * argv)
{
    shell_printf("PIT Test CMD\r\n");
    PIT_QuickInit(HW_PIT0_CH0, 1000*500);
    PIT_CallbackInstall(HW_PIT0_CH0, PIT0_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH0, kPIT_IT_TOF);
    
    PIT_QuickInit(HW_PIT0_CH1, 1000*1000);
    PIT_CallbackInstall(HW_PIT0_CH1, PIT1_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH1, kPIT_IT_TOF);
    
    PIT_QuickInit(HW_PIT0_CH2, 1000*2000);
    PIT_CallbackInstall(HW_PIT0_CH2, PIT2_CallBack);
    PIT_ITDMAConfig(HW_PIT0_CH2, kPIT_IT_TOF);
    
    PIT_QuickInit(HW_PIT0_CH3, 1000*4000);
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
