#include "shell.h"
#include "clock.h"
#include "common.h"
#include "lptmr.h"


void LPTMR_ISR(void)
{
    static uint32_t cnt;
    shell_printf("LPTMR INT ENTER%d\r\n", cnt++);
}

int CMD_LPTMR(int argc, char * const * argv)
{
    uint32_t i;
    shell_printf("LPTMR TEST CMD\r\n");
    LPTMR_TC_InitTypeDef LPTMR_TC_InitStruct1;
    LPTMR_TC_InitStruct1.instance = HW_LPTMR0;
    LPTMR_TC_InitStruct1.timeInMs = 500;
    LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
    LPTMR_ITDMAConfig(kLPTMR_IT_TOF);
    LPTMR_CallbackInstall(LPTMR_ISR);
    
    
    LPTMR_PC_QuickInit(LPTMR_ALT2_PC05);
    
    while(1)
    {
        i = LPTMR_PC_ReadCounter();
        shell_printf("ReadValue:%d\r\n", i);
        LPTMR_ClearCount();
        DelayMs(1000);
        
        
    }
    return 0;
}

const cmd_tbl_t CommandFun_LPTMR = 
{
    .name = "LPTMR",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_LPTMR,
    .usage = "LPTMR TEST",
    .complete = NULL,
    .help = "\r\n"
};
