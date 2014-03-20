#include "shell.h"
#include "rtc.h"
#include "common.h"
#include "systick.h"



int CMD_RTC(int argc, char * const argv[])
{
    printf("RTC TEST\r\n");
    uint8_t last_sec;
    RTC_Init();
    RTC_CalanderTypeDef RTC_Calander1;
    RTC_Calander1.Hour = 10;
    RTC_Calander1.Minute = 57;
    RTC_Calander1.Second = 58;
    RTC_Calander1.Month = 10;
    RTC_Calander1.Date = 10;
    RTC_Calander1.Year = 2013;
    
    //RTC_SetCalander(&RTC_Calander1);
    NVIC_EnableIRQ(RTC_IRQn);
    while(1) 
    {
        RTC_GetCalander(&RTC_Calander1); //∂¡»° ±º‰
        if(last_sec != RTC_Calander1.Second)
        {
        printf("%d-%d-%d %d:%d:%d\r\n", RTC_Calander1.Year, RTC_Calander1.Month, RTC_Calander1.Date, RTC_Calander1.Hour, RTC_Calander1.Minute, RTC_Calander1.Second);
            last_sec = RTC_Calander1.Second;
        }	
    }
}

void RTC_IRQHandler(void)
{
    RTC_SecondIntProcess();
}

const cmd_tbl_t CommandFun_RTC = 
{
    .name = "RTC",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_RTC,
    .usage = "RTC",
    .complete = NULL,
    .help = "RTC"
};
