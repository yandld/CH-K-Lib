#include "chlib_k.h"
#include "fifo.h"
#include "boarddef.h"
#include "DataScope_DP.h"

uint16_t gFIFOBuf[16][64];

static struct FIFO fifo[16];
    
const int gADC_InstanceTable[] =  ADC_PORTS;
const int gADC_ChnTable[] = ADC_CHANNELS;

static void PIT0_ISR(void)
{
    uint16_t val;
    static int chl;
    static int i;
    val = ADC_ReadValue(gADC_InstanceTable[chl], kADC_MuxA);
    
    fifo_put(&fifo[chl], val);
    
    ADC_StartConversion(gADC_InstanceTable[chl], gADC_ChnTable[chl], kADC_MuxA);
    
    chl++;
    chl%=16;
}

int main(void)
{
    uint32_t i, cnt;
    
    // basic init
    DelayInit();
    UART_QuickInit(BOARD_UART_MAP, 115200);
    UART_QuickInit(RS485_CH1_MAP, 115200);
    UART_QuickInit(RS485_CH2_MAP, 115200);
    UART_QuickInit(RS485_CH3_MAP, 115200);
    
    // start running systick
    SYSTICK_Init(1000*1000);
    SYSTICK_Cmd(true);
    
    CLOCK_GetClockFrequency(kCoreClock, &i);
    printf("CoreClock:%d\r\n", i);
    
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
        DataScope_Get_Channel_Data( 1.0,  1 ); //??? 1.0  ???? 1
        DataScope_Get_Channel_Data( 2.0 , 2 ); //??? 2.0  ???? 2
        DataScope_Get_Channel_Data( 3.0 , 3 ); //??? 3.0  ???? 3
        DataScope_Get_Channel_Data( 4.0 , 4 ); //??? 4.0  ???? 4
        DataScope_Get_Channel_Data( 5.0 , 5 ); //??? 5.0  ???? 5
        DataScope_Get_Channel_Data( 6.0 , 6 ); //??? 6.0  ???? 6
        DataScope_Get_Channel_Data( 7.0 , 7 ); //??? 7.0  ???? 7
        DataScope_Get_Channel_Data( 8.0 , 8 ); //??? 8.0  ???? 8
        DataScope_Get_Channel_Data( 9.0 , 9 ); //??? 9.0  ???? 9
        DataScope_Get_Channel_Data( 10.0 , 10); //??? 10.0 ???? 10
        
        cnt = DataScope_Data_Generate(10); //??10???? ??????,???????
		
        for( i = 0 ; i < cnt; i++)  //????,??????   
        {
            UART_WriteByte(HW_UART0, DataScope_OutPut_Buffer[i]);  
        }  
        
        DelayMs(5);
    }
}


