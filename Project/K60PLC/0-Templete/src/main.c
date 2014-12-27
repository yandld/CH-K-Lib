#include "chlib_k.h"
#include "dsp_basic_demo.h"
#include "dsp_fast_demo.h"
#include "dsp_complex_demo.h"
#include "main.h"

uint32_t fftSize = 32; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1;
int ix;
int main(void)
{
	SYSTICK_Init(1000*1000);
    SYSTICK_Cmd(true);
   
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("System start!\r\n");
	
    /**实数信号转换成复数信号*/
	for(int i=0;i<31;i++) {
	 test_input[2*i] = test_signal[i];
	 test_input[2*i+1] = 0;		
	}
	/**转换后的输出测试*/
//	printf("y = [");
//	for(int i=0;i<64;i++) {
//		printf("%f ",test_input[i]);
//	}
//	printf("]\n\r");
	 int  xx = SYSTICK_GetVal();

	arm_cfft_f32(&arm_cfft_sR_f32_len32, test_input, ifftFlag, doBitReverse);
	arm_cmplx_mag_f32(test_input, test_output, fftSize);
    printf("ADC_QuickReadValue cost: %dticks\r\n", xx - SYSTICK_GetVal());

	printf("y = [");
	for(int i=0;i<32;i++) {
		printf("%f ",test_output[i]);
	}
	printf("];\n\r");
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 8);
        DelayMs(500);
    }
}


