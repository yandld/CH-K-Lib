

#include <math.h>

#include "filter.h"



#define M_PI 3.141592653f
#define ACC_LPF_CUT 10.0f		//加速度低通滤波器截止频率10Hz


/**
 * @brief  一节低通滤波器系数计算
 * @param  time: 调用时间 delataT
 * @param  f_cut: 截止频率
 * @retval 参数值
 */
float lpf_1st_factor_cal(float time, float f_cut)
{
    return time / (time + 1 / (2 * M_PI * f_cut));
}


float lpf_1st(float old_data, float new_data, float factor)
{
	return old_data * (1 - factor) + new_data * factor; 
}




