#include "shell.h"
#include "rtc.h"
#include "common.h"
#include "systick.h"

void RTC_ISR(void)
{
    printf("RTC INT\r\n");
}

int CMD_RTC(int argc, char * const argv[])
{
    printf("RTC TEST\r\n");
    
    RTC_DateTime_Type time_stamp;
    uint32_t tsr;

    time_stamp.year = 2014;
    time_stamp.month = 3;
    time_stamp.day = 20;
    time_stamp.hour = 0;
    time_stamp.minute = 0;
    time_stamp.second = 15;
    RTC_QuickInit(&time_stamp);
    //RTC_ITDMAConfig(kRTC_IT_TimeOverflow);
    //RTC_CallbackInstall(RTC_ISR);
    while(1)
    {
        RTC_GetDateTime(&time_stamp);
        printf("%d-%d-%d %d:%d:%d\r\n", time_stamp.year, time_stamp.month, time_stamp.day, time_stamp.hour, time_stamp.minute, time_stamp.second);
        DelayMs(1000);
    }
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
