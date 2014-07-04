#include "ftm.h"
#include "shell.h"
#include "gpio.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include "systick.h"
#include "common.h"

static int CMD_FTM_EA(int argc, char *const argv[])
{
    uint32_t req;
    uint32_t value = 0;
    uint8_t instance = 0;
    req = strtoul(argv[2], 0, 0);
    instance = FTM_PWM_QuickInit(FTM2_CH1_PB19 ,kPWM_EdgeAligned, 5000);
    //FTM_PWM_InvertPolarity(instance, HW_FTM_CH1, kFTM_PWM_LowTrue);
    while(1)
    {
       // FTM_PWM_ChangeDuty(instance, HW_FTM_CH1, value++);
        DelayMs(1);
    }
     
    return 0;
}

void FTM_ISR(void)
{
    uint32_t val;
    uint32_t clock;
    val = FTM_GetChlCounter(HW_FTM1, HW_FTM_CH0);
    /* clear module counter */
    FTM_SetMoudleCounter(HW_FTM1, 0);
    CLOCK_GetClockFrequency(kBusClock, &clock);
    val = (clock/128)/val;
    printf("FTM_ISR%dHz     \r", val);
}

static int CMD_FTM_IC(int argc, char *const argv[])
{
    FTM_PWM_QuickInit(FTM0_CH3_PA06, kPWM_EdgeAligned, 200);
    
    FTM_IC_QuickInit(FTM1_CH0_PA08, kFTM_ClockDiv128);
    FTM_IC_SetTriggerMode(HW_FTM1, HW_FTM_CH0, kFTM_IC_FallingEdge);
    FTM_CallbackInstall(HW_FTM1, FTM_ISR);
    FTM_ITDMAConfig(HW_FTM1, kFTM_IT_CH0, true);
    return 0;
}

static int CMD_FTM_BUZZER(int argc, char *const argv[])
{
    uint8_t instance = 0;
    uint32_t req;
    QuickInit_Type QuickInitStruct1;
    req = strtoul(argv[2], 0, 0);
    if(!req)
    {
        req = 1000;
    }
    printf("BUZZER REQ:%dHZ\r\n", req);
    instance = FTM_PWM_QuickInit(BOARD_BUZZER_FTM_MAP ,kPWM_EdgeAligned, req);
    QuickInitDecode(BOARD_BUZZER_FTM_MAP, &QuickInitStruct1);
    FTM_PWM_ChangeDuty(instance, QuickInitStruct1.channel, 5000); 
    return 0;
}

static int CMD_FTM_COMBINE(int argc, char *const argv[])
{
    uint8_t instance = 0;
    uint32_t req;
    uint32_t value;
    uint32_t duty;
    req = strtoul(argv[2], 0, 0);
    instance = FTM_PWM_QuickInit(FTM2_CH0_PB18, kPWM_Complementary, 1000);
    instance = FTM_PWM_QuickInit(FTM2_CH1_PB19, kPWM_Complementary, 1000);
    // FTM_PWM_InvertPolarity(instance, HW_FTM_CH0, kFTM_PWM_HighTrue);
    FTM_PWM_ChangeDuty(instance, HW_FTM_CH0, 8000);
    DelayMs(100);
    FTM_PWM_ChangeDuty(instance, HW_FTM_CH1, 3000);
    return 0;
}

static int CMD_FTM_QD(int argc, char *const argv[])
{
    int32_t value;
    uint8_t dir; 
    uint32_t instance;
    instance = FTM_QD_QuickInit(FTM1_QD_PHA_PA08_PHB_PA09, kFTM_QD_NormalPolarity, kQD_PHABEncoding);
    while(1)
    {
        FTM_QD_GetData(instance, &value, &dir);
        printf("value:%6d dir:%d\r", value, dir);
        DelayMs(20);            
    }
}


int CMD_FTM(int argc, char *const argv[])
{
    printf("FTM TEST\r\n");
    //quick init
    if((argc >= 2) && (!strcmp(argv[1], "EA")))
    {
        return CMD_FTM_EA(argc, argv);
    }
    if((argc >= 2) && (!strcmp(argv[1], "COMBINE")))
    {
        return CMD_FTM_COMBINE(argc, argv);
    }
    if((argc >= 2) && (!strcmp(argv[1], "BUZZER")))
    {
        return CMD_FTM_BUZZER(argc, argv);
    }
    if((argc >= 2) && (!strcmp(argv[1], "QD")))
    {
        return CMD_FTM_QD(argc, argv);
    }
    if((argc >= 2) && (!strcmp(argv[1], "IC")))
    {
        return CMD_FTM_IC(argc, argv);
    }
    
    return 0;
}

SHELL_EXPORT_CMD(CMD_FTM, FTM, ftm test);
