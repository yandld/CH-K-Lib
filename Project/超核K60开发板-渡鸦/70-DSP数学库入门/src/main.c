#include "chlib_k.h"
#include "arm_math.h"

/*
常用函数类
 ### BasicMathFunctions:
 * arm_abs_f32
 * arm_add_f32
 * arm_dot_prod_f32 // 向量数量积 
 * arm_muti_f32
 * arm_negate_f32
 * arm_offset_f32
 * arm_scale_f32
 * arm_shift_f32
 * arm_sub_f32
 ### ComplexMathFunctions:
 * arm_cmplx_conj_f32
 * arm_cmplx_dot_parod_f32
 * arm_cmplx_mag_f32
 * arm_cmplx_mag_squared_f32
 * arm_cmplx_muti_cmplx_f32
 * arm_cmplx_muti_real_f32
 ### ControllerFunctions:
 * arm_pid_init_f32
 * arm_pid_reset_f32
 * arm_sin_cos_f32
 ### FastMathFunctions:
 * arm_cos_f32
 * arm_sin_f32
 * arm_sqrt_f32
 ### FilteringFunctions:

 ### MatrixFunctions:
 ### StatisticsFunctions:
 * arm_max_f32
 * arm_mean_f32
 * arm_min_f32
 * arm_power_f32
 * arm_rms_f32
 * arm_std_f32
 * arm_var_f32
 ### SupportFunctions:
 * arm_copy_f32
 * arm_fill_f32
 ### TransformFunctions:

*/

#define PA  (3.1415926)

int main(void)
{
    uint32_t clock;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);

    float32_t in = (PA*PA);
    float32_t out;
    /* 做一个开根号测试 */
    arm_sqrt_f32(in, &out);
    printf("arm_sqrt_f32:sqrt(%f) = %f\r\n",in, out);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


