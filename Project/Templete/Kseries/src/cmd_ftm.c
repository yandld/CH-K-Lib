#include "ftm.h"
#include "shell/shell.h"
#include "gpio.h"




int CMD_FTM(int argc, char * const * argv)
{
    uint8_t instance = 0;
    uint32_t req;

    printf("FTM TEST\r\n");
    //quick init
    if(!strcmp(argv[1], "-q"))
    {
        req = strtoul(argv[2], 0, 0);
        instance = FTM_QuickInit(FTM2_CH1_PB19 ,req);
        FTM_PWM_ChangeDuty(instance, 1, 3000); 
    }
    //combie
    if(!strcmp(argv[1], "-c"))
    {
        req = strtoul(argv[2], 0, 0);
        FTM_InitTypeDef FTM_InitStruct1;
        instance = HW_FTM2;
        FTM_InitStruct1.instance = instance;
        FTM_InitStruct1.frequencyInHZ = 320;
        FTM_InitStruct1.chl = HW_FTM_CH1;
        FTM_InitStruct1.mode = kPWM_Combine;
        FTM_Init(&FTM_InitStruct1);
        
        FTM_InitStruct1.instance = instance;
        FTM_InitStruct1.frequencyInHZ = 320;
        FTM_InitStruct1.chl = HW_FTM_CH0;
        FTM_InitStruct1.mode = kPWM_Combine;
        FTM_Init(&FTM_InitStruct1);
        
        PORT_PinMuxConfig(HW_GPIOB, 18, kPinAlt3);
        PORT_PinMuxConfig(HW_GPIOB, 19, kPinAlt3);
        
        FTM_PWM_ChangeDuty(instance, 1, 3000); 
    }
   
    return 0;
}



const cmd_tbl_t CommandFun_FTM = 
{
    .name = "FTM",
    .maxargs = 4,
    .repeatable = 1,
    .cmd = CMD_FTM,
    .usage = "FTM <requerency>",
    .complete = NULL,
    .help = "FTM <requerency>"
};
