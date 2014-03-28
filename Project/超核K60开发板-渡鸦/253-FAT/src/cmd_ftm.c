#include "ftm.h"
#include "shell.h"
#include "gpio.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include "systick.h"

static int CMD_FTM_EA(int argc, char *const argv[])
{
    uint8_t instance = 0;
    uint32_t req;
    req = strtoul(argv[2], 0, 0);
    instance = FTM_PWM_QuickInit(FTM2_CH1_PB19 , 3140);
    //FTM_PWM_InvertPolarity(instance, HW_FTM_CH1, kFTM_PWM_LowTrue);
    FTM_PWM_ChangeDuty(instance, 1, 3000); 
    DelayMs(100);
    FTM_PWM_ChangeDuty(instance, 1, 5000); 
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
    instance = FTM_PWM_QuickInit(BOARD_BUZZER_FTM_MAP ,req);
    QuickInitDecode(BOARD_BUZZER_FTM_MAP, &QuickInitStruct1);
    FTM_PWM_ChangeDuty(instance, QuickInitStruct1.channel, 5000); 
    return 0;
}

static int CMD_FTM_COMBINE(int argc, char *const argv[])
{
    uint8_t instance = 0;
    uint32_t req;
    uint32_t value;
    req = strtoul(argv[2], 0, 0);
    FTM_PWM_InitTypeDef FTM_InitStruct1;
    instance = HW_FTM2;
    FTM_InitStruct1.instance = instance;
    FTM_InitStruct1.frequencyInHZ = req;
    FTM_InitStruct1.chl = HW_FTM_CH1;
    FTM_InitStruct1.mode = kPWM_Complementary;
    FTM_PWM_Init(&FTM_InitStruct1);
        
    FTM_InitStruct1.instance = instance;
    FTM_InitStruct1.frequencyInHZ = req;
    FTM_InitStruct1.chl = HW_FTM_CH0;
    FTM_InitStruct1.mode = kPWM_Complementary;
        
    FTM_PWM_Init(&FTM_InitStruct1);
        
    PORT_PinMuxConfig(HW_GPIOB, 18, kPinAlt3);
    PORT_PinMuxConfig(HW_GPIOB, 19, kPinAlt3);
    // FTM_PWM_InvertPolarity(instance, HW_FTM_CH0, kFTM_PWM_HighTrue);
    FTM_PWM_ChangeDuty(instance, HW_FTM_CH0, 7500);
    DelayMs(100);
    FTM_PWM_ChangeDuty(instance, HW_FTM_CH1, 3500);
}

static int CMD_FTM_QD(int argc, char *const argv[])
{
    uint32_t value;
    uint8_t dir; 
    uint32_t instance;
    instance = FTM_QD_QuickInit(FTM1_QD_PHA_PA08_PHB_PA09);
    while(1)
    {
        FTM_QD_GetData(instance, &value, &dir);
        printf("value:%d dir:%d\r\n", value, dir);
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
    return 0;
}



const cmd_tbl_t CommandFun_FTM = 
{
    .name = "FTM",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_FTM,
    .usage = "FTM <CMD> (CMD = EA,COMBINE,QD,BUZZER <req>)",
    .complete = NULL,
    .help = "FTM <CMD> (CMD = EA,COMBINE,QD,BUZZER <req>)",
};
