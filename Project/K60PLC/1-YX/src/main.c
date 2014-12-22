#include "chlib_k.h"
#include "fifo.h"
#include "boarddef.h"


volatile uint16_t gFIFOBuf[16][64];
volatile uint16_t *pADCChl[16];
volatile static int cnt;
static struct FIFO fifo[16];
    
const int gADC_InstanceTable[] =   ADC_PORTS;
const int gADC_ChnTable[] =        ADC_CHANNELS;

static void PIT0_ISR(void)
{
    static int chl;
    static int i;
    *pADCChl[chl] = ADC_ReadValue(gADC_InstanceTable[chl], kADC_MuxA);
    
    ADC_StartConversion(gADC_InstanceTable[chl], gADC_ChnTable[chl], kADC_MuxA);
    
    chl++;
    chl%=16;
    
    cnt++;
}

int main(void)
{
    uint32_t i;
    DelayInit();
    SYSTICK_Init(1000*1000);
    SYSTICK_Cmd(true);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("System start!\r\n");
    
    //init LED
    GPIO_QuickInit(LED0_PORT, LED0_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED1_PORT, LED1_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED2_PORT, LED2_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED3_PORT, LED3_PIN, kGPIO_Mode_OPP);

    // Init ADC 
    ADC_InitTypeDef ADC_InitStruct;

    for(i = 0; i < 16; i++)
    {
        ADC_InitStruct.instance = gADC_InstanceTable[i];
        ADC_InitStruct.clockDiv = kADC_ClockDiv1;
        ADC_InitStruct.resolutionMode = kADC_SingleDiff12or13;
        ADC_InitStruct.triggerMode = kADC_TriggerSoftware;
        ADC_InitStruct.singleOrDiffMode = kADC_Single;
        ADC_InitStruct.continueMode = kADC_ContinueConversionDisable;
        ADC_InitStruct.hardwareAveMode = kADC_HardwareAverageDisable;
        ADC_InitStruct.vref = kADC_VoltageVREF;
        ADC_Init(&ADC_InitStruct);   
    }

    for(i = 0; i < 16; i++)
    {
    //    pADCChl[i] = &gADCBuf[i][0];
    }
    
    // init PIT , 833US
    PIT_QuickInit(HW_PIT_CH0, 833); 
    PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
    
    // test ADC conversion time 
    i = SYSTICK_GetVal();
    ADC_QuickReadValue(ADC1_SE0_DP0);
    printf("ADC_QuickReadValue cost: %dticks\r\n", i - SYSTICK_GetVal());
    
    // init and test FIFO system
    for(i = 0; i < 16; i++)
    {
        fifo_init(&fifo[i], 32,  &gFIFOBuf[i][0]);
    }
    fifo_test();
    
    while(1)
    {
        LED0 = !LED0;
        LED1 = !LED1;
        LED2 = !LED2;
        LED3 = !LED3;
        printf("%d\r\n", cnt);
        cnt = 0;
        DelayMs(500);
    }
}


