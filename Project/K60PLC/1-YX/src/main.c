
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
float32_t gFFTRealBuf[16][32];		//实数信号矩阵
float32_t gComplexSignal[16][64];	//复数信号矩阵	准备进行FFT的复数信号
float32_t gFFTResaultBuf[16][64];	//FFT结果矩阵
float32_t gFFTModuleBuf[16][32];	//FFT频谱矩阵
float32_t 	gSig50HzReal[16],	gSig50HzImage[16],	/**50Hz 主波实部和虚部*/
			gSig100HzReal[16],	gSig100HzImage[16],	/**100Hz 二次谐波实部和虚部*/
			gSig150HzReal[16],	gSig150HzImage[16];	/**150Hz 三次谐波实部和虚部*/
			
float32_t 	gSig50HzModule[16],gSig100HzModule[16],gSig150HzModule[16];	/**50Hz 100Hz 150Hz 复数模*/

static struct FIFO fifo[16];
const int gADC_InstanceTable[] =  ADC_PORTS;
const int gADC_ChnTable[] = ADC_CHANNELS;

/***************FFT use*********************/
uint32_t fftSize = 32; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1;
/*******************************************/
/**test only */
//float32_t test_signal_50 [32] = 
//{
//	-0.0670,0.1541,0.3697,0.5607,0.7342,0.9073,1.0851,1.2500,1.3668,1.4024,1.3445,
//	 1.2071,1.0192,0.8026,0.5547,0.2500,-0.1389,-0.6112,-1.1168,-1.5607,-1.8349,-1.8644,
//	 -1.6446,-1.2500,-0.8074,-0.4453,-0.2438,-0.2071,-0.2721,-0.3455,-0.3488,-0.2500
//}; 
//float32_t outCalData[64] = {0.0f};
//float32_t outCalData2[64] = {0.0f};
/************************************/
static void normalization (uint16_t *data,uint16_t len,float32_t *out) {
	uint16_t AdcDefaultValue = 2048;
	for(int i=0;i<len;i++) {
		out[i] = ((float32_t)((int32_t)data[i]-AdcDefaultValue)/4096)*2.5;
	}
}
static int32_t realToComplex(float32_t *in,uint32_t length,float32_t *out){

	for(int32_t i=0;i<length;i++) {
		out[2*i] = in[i];		//real parts
		out[2*i+1] = 0.0f;		//image parts
	}
	return 0;
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
		/*******FFT Cal*************/
		normalization(&gFIFOBuf[i][0],32,&gComplexSignal[i][0]);
		realToComplex(&gFFTRealBuf[i][0],32,&gComplexSignal[i][0]);
		arm_cfft_f32(&arm_cfft_sR_f32_len32, &gComplexSignal[i][0], ifftFlag, doBitReverse);
		for(uint8_t j=0;j<32;j++) {
			gFFTResaultBuf[i][j] = gComplexSignal[i][j];
			gComplexSignal[i][j] = 0; /**claen buffer*/
		}
		arm_cmplx_mag_f32(&gFFTResaultBuf[i][0], &gFFTModuleBuf[i][0], fftSize);	//get 32 complex modules
		/**save 1st 2nd 3rd stages real parts &image parts*/
		gSig50HzReal[i] = gFFTResaultBuf[i][0];		gSig50HzImage[i] = gFFTResaultBuf[i][1];
		gSig100HzReal[i] = gFFTResaultBuf[i][2];	gSig100HzImage[i] = gFFTResaultBuf[i][3];
		gSig150HzReal[i] = gFFTResaultBuf[i][4];	gSig150HzImage[i] = gFFTResaultBuf[i][5];
		/**save 1st 2nd 3rd stages modules*/
		gSig50HzModule[i] = gFFTModuleBuf[i][1];
		gSig100HzModule[i] = gFFTModuleBuf[i][2];
		gSig150HzModule[i] = gFFTModuleBuf[i][3];;
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
    TestRS485IntSend();
    
    
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


