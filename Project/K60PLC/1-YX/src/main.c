
// dirver lib
#include "chlib_k.h"

#include "fifo.h"
#include "mb85rc64.h"

// board pin define
#include "boarddef.h"
#include "DataScope_DP.h"

#include "unit_test.h"

/**
	Sample rate 	= 1.6 Ks/s
	Sample point 	= 32 
	FFT Speed		= 3846 HardwareCycle(@120 MHz) = 32 us
	NOTE : DO NOT CHANGE PIT0_TIME_IN_US VALUE!
*/
#define PIT0_TIME_IN_US    (625)
/**console check*/
#define PIT1_TIME_IN_US    (50*1000)

// global vars
uint16_t gFIFOBuf[16][32];
static struct FIFO fifo[16];
const int gADC_InstanceTable[] =  ADC_PORTS;
const int gADC_ChnTable[] = ADC_CHANNELS;

/************************************/
/**test space*/
/************************************/
static void normalization (uint32_t *data,uint32_t len,float32_t *out) {
	for(int i=0;i<len;i++) {
		out[i] = ((float32_t)((int32_t)data[i]-2048)/4096)*2.5;
	}
}

//PIT0 中断
static void PIT0_ISR(void)
{
    uint16_t val;
    int i;
    
    for(i=0;i<16;i++)
    {
        //ADC sampling and store
        ADC_StartConversion(gADC_InstanceTable[i], gADC_ChnTable[i], kADC_MuxA);
        while(ADC_IsConversionCompleted(gADC_InstanceTable[i], kADC_MuxA) == 1);
        val = ADC_ReadValue(gADC_InstanceTable[i], kADC_MuxA);
        fifo_put(&fifo[i], val);
    }
    
}

//PIT 1 中断
static void PIT1_ISR(void)
{
    // print data
    int i, index;
    for(i=0;i<16;i++)
    {
        index = fifo_get(&fifo[i]);
        printf("%d ", gFIFOBuf[i][index]);
    }
    printf("\r");
    
}

int main(void)
{
    uint32_t i, cnt;
    int r;
    
    // UART and Delay init
    DelayInit();
    UART_QuickInit(BOARD_UART_MAP, 115200);
    UART_QuickInit(RS485_CH1_MAP, 115200);
    UART_QuickInit(RS485_CH2_MAP, 115200);
    UART_QuickInit(RS485_CH3_MAP, 115200);
    
    // init and test FIFO system
    for(i = 0; i < 16; i++)
    {
        fifo_init(&fifo[i], 32,  &gFIFOBuf[i][0]);
    }
    
    // start running systick
    SYSTICK_Init(1000*1000);
    SYSTICK_Cmd(true);
        
    //init LED
    GPIO_QuickInit(LED0_PORT, LED0_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED1_PORT, LED1_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED2_PORT, LED2_PIN, kGPIO_Mode_OPP);
    GPIO_QuickInit(LED3_PORT, LED3_PIN, kGPIO_Mode_OPP);

    // init RTC
    RTC_QuickInit();

    //测试系统
    TestClock();
    TestADCTime();
    TestLED();
    TestRS485Receive();
    TestMB85RC64();
    TestRTC();
    TestFFTTime();
    
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
    
    // init PIT
    PIT_QuickInit(HW_PIT_CH0, PIT0_TIME_IN_US); 
    PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
    printf("%dus Interrupt Enabled\r\n", PIT0_TIME_IN_US);
    
    // init PIT
    PIT_QuickInit(HW_PIT_CH1, PIT1_TIME_IN_US); 
    PIT_CallbackInstall(HW_PIT_CH1, PIT1_ISR);
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, true);
    printf("%dus Interrupt Enabled\r\n", PIT1_TIME_IN_US);

	
    while(1)
    {

    }
}


