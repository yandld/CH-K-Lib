#include "shell.h"
#include "wdog.h"
#include "common.h"
#include "systick.h"

void WDOG_ISR(void)
{
    shell_printf("WDOG ISR\r\n");
    
}



int CMD_WDOG(int argc, char * argv[])
{
    printf("WDOG TEST\r\n");
    printf("WDOG RESET COUNTER:%d\r\n", WDOG_ReadResetCounter());
    WDOG_InitTypeDef WDOG_InitStruct1;
    WDOG_InitStruct1.mode = kWDOG_Mode_Window;
    WDOG_InitStruct1.timeOutInMs = 2000;
    WDOG_InitStruct1.windowInMs = 1800;
    NVIC_EnableIRQ(Watchdog_IRQn);
    WDOG_Init(&WDOG_InitStruct1);
    //貌似进不去中断
    WDOG_CallbackInstall(WDOG_ISR);
    WDOG_ITDMAConfig(ENABLE);
    
    while(1)
    {
        DelayMs(400);
        printf("WDOG LOOP!\r\n");
        WDOG_Refresh();
        
    }
    
}



const cmd_tbl_t CommandFun_WDOG = 
{
    .name = "WDOG",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_WDOG,
    .usage = "WDOG",
    .complete = NULL,
    .help = "WDOG"
};
