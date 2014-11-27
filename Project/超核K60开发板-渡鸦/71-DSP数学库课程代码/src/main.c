#include "chlib_k.h"
#include "dsp_basic_demo.h"
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
//	arm_abs_f32_demo();			//abs demo
//	arm_add_f32_demo();			//add demo
//	arm_dot_prod_f32_demo();	//dot prod demo
//	arm_mult_f32_demo();		//mult demo
//	arm_negate_f32_demo();		//negate demo
//	arm_offset_f32_demo();		//offset demo
//	arm_scale_f32_demo();		//scale demo
	arm_sub_f32_demo();

    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


