/**
  ******************************************************************************
  * @file    rtc.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
 #include "rtc.h"
 #include "common.h"
 
 //!< Callback function slot
static RTC_CallBackType RTC_CallBackTable[1] = {NULL};
 
#define SECONDS_IN_A_DAY     (86400U)
#define SECONDS_IN_A_HOUR    (3600U)
#define SECONDS_IN_A_MIN     (60U)
#define DAYS_IN_A_YEAR       (365U)
#define DAYS_IN_A_LEAP_YEAR  (366U)
// Table of month length (in days) for the Un-leap-year
static const uint8_t ULY[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
// Table of month length (in days) for the Leap-year
static const uint8_t  LY[] = {0U, 31U, 29U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
// Number of days from begin of the non Leap-year
static const uint16_t MONTH_DAYS[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};


// 如果RTC时钟有效 则忽略timedate参数 否则写入timedate
void RTC_QuickInit(RTC_DateTime_Type* timedate)
{
    RTC_InitTypeDef RTC_InitStruct1;
    RTC_InitStruct1.initialDateTime = timedate;
    RTC_InitStruct1.isUpdate = false; //will not force to update
    RTC_InitStruct1.oscLoad = kRTC_OScLoad_16PF;
    RTC_Init(&RTC_InitStruct1);
}

int RTC_GetWeekFromYMD(int year, int month, int days)
{  
    static int mdays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 };  
    int i, y = year - 1;  
    for (i=0; i<month; ++i) days += mdays[i];  
    if (month > 2) 
    { // Increment date if this is a leap year after February  
        if (((year%400) == 0) || ((year&3) == 0 && (year%100))) ++days;  
    }  
    return (y+y/4-y/100+y/400+days)%7;  
}

static void RTC_DateTimeToSecond(const RTC_DateTime_Type * datetime, uint32_t * seconds)
{
    /* Compute number of days from 1970 till given year*/
    *seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    *seconds += ((datetime->year / 4) - (1970U / 4));
    /* Add number of days till given month*/
    *seconds += MONTH_DAYS[datetime->month];
    /* Add days in given month*/
    *seconds += datetime->day;
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U))
    {
        (*seconds)--;
    }

    *seconds = ((*seconds) * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) + 
               (datetime->minute * SECONDS_IN_A_MIN) + datetime->second;
    (*seconds)++;
}

static void RTC_SecondToDateTime(const uint32_t * seconds, RTC_DateTime_Type * datetime)
{
    uint32_t x;
    uint32_t Seconds, Days, Days_in_year;
    const uint8_t *Days_in_month;
    /* Start from 1970-01-01*/
    Seconds = *seconds;
    /* days*/
    Days = Seconds / SECONDS_IN_A_DAY;
    /* seconds left*/
    Seconds = Seconds % SECONDS_IN_A_DAY;
    /* hours*/
    datetime->hour = Seconds / SECONDS_IN_A_HOUR;
    /* seconds left*/
    Seconds = Seconds % SECONDS_IN_A_HOUR;
    /* minutes*/
    datetime->minute = Seconds / SECONDS_IN_A_MIN;
    /* seconds*/
    datetime->second = Seconds % SECONDS_IN_A_MIN;
    /* year*/
    datetime->year = 1970;
    Days_in_year = DAYS_IN_A_YEAR;

    while (Days > Days_in_year)
    {
        Days -= Days_in_year;
        datetime->year++;
        if  (datetime->year & 3U)
        {
            Days_in_year = DAYS_IN_A_YEAR;
        }
        else
        {
            Days_in_year = DAYS_IN_A_LEAP_YEAR;    
        }
    }

    if  (datetime->year & 3U)
    {
        Days_in_month = ULY;
    }
    else
    {
        Days_in_month = LY;    
    }

    for (x=1U; x <= 12U; x++)
    {
        if (Days <= (*(Days_in_month + x)))
        {
            datetime->month = x;
            break;
        }
        else
        {
            Days -= (*(Days_in_month + x));
        }
    }
    datetime->day = Days;
}

void RTC_GetDateTime(RTC_DateTime_Type * datetime)
{
    if(!datetime)
    {
        return;
    }
    uint32_t i = RTC->TSR;
    RTC_SecondToDateTime(&i, datetime);
}

void RTC_Init(RTC_InitTypeDef * RTC_InitStruct)
{
    uint32_t i;
    SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;
    // RTC->CR = 0;
    // OSC load config
    switch(RTC_InitStruct->oscLoad)
    {
        case kRTC_OScLoad_2PF:
            RTC->CR |= RTC_CR_SC2P_MASK;
            break;
        case kRTC_OScLoad_4PF:
            RTC->CR |= RTC_CR_SC4P_MASK;
            break;
        case kRTC_OScLoad_8PF:
            RTC->CR |= RTC_CR_SC8P_MASK;
            break;
        case kRTC_OScLoad_16PF:
            RTC->CR |= RTC_CR_SC16P_MASK;
            break;
        default:
            break;
    }
    // see if we have to reconfig TSR
    if(((!RTC->TSR) || RTC_InitStruct->isUpdate) && (RTC_InitStruct->initialDateTime != NULL))
    {
        #ifdef DEBUG
        printf("RTC_Init() - Reconfig!\r\n");
        #endif
        RTC_DateTimeToSecond(RTC_InitStruct->initialDateTime, &i);
        RTC->SR &= ~RTC_SR_TCE_MASK;
        RTC->TSR = RTC_TSR_TSR(i);
        RTC->SR |= RTC_SR_TCE_MASK;
    }
    // enable OSC
    RTC->CR |= RTC_CR_OSCE_MASK;
	for(i=0;i<0x600000;i++) {};
    // enable RTC
    RTC->SR |= RTC_SR_TCE_MASK;
}

void RTC_ITDMAConfig(RTC_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kRTC_IT_TimeOverflow_Disable:
            RTC->IER &= ~RTC_IER_TOIE_MASK;
            break;
        case kRTC_IT_TimeOverflow:
            RTC->IER |= RTC_IER_TOIE_MASK;
            NVIC_EnableIRQ(RTC_IRQn);
            break;
        case kRTC_IT_TimeAlarm_Disable:
            RTC->IER &= ~RTC_IER_TAIE_MASK;
            break;
        case kRTC_IT_TimeAlarm:
            RTC->IER |= RTC_IER_TAIE_MASK;
            NVIC_EnableIRQ(RTC_IRQn);
            break;
        default:
            break;
    }
}

void RTC_CallbackInstall(RTC_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        RTC_CallBackTable[0] = AppCBFun;
    }
}

void RTC_IRQHandler(void)
{
    if(RTC_CallBackTable[0])
    {
        RTC_CallBackTable[0]();
    }
}

