#ifndef _MAIN_H
#define _MAIN_H

#include "gpio.h"
#include "common.h"
#include "uart.h"

/* DSP lib */
#define ARM_MATH_CM4
#include "arm_math.h"
#include "arm_const_structs.h"

/**	测试波形*/
/**	采样率：1KHz
	fs = 1 KHz
	ts = 0.001 s
	采样点数：100
	ns = 100
	波形：正弦波
	周期：50Hz
	相位：0
	峰峰值（2V）
	生成方式： Matlab
	信号维度 1维时间信号（无虚部）
	n*/
float32_t test_signal[32] = {
0		,0.3090	,0.5878	,0.8090	,0.9511	,1.0000	,0.9511	,0.8090,  
0.5878	,0.3090	,0.0000	,-0.3090,-0.5878,-0.8090,-0.9511,-1.0000,
-0.9511	,-0.8090,-0.5878,-0.3090,-0.0000,0.3090	,0.5878	,0.8090,
0.9511 	,1.0000	,0.9511	,0.8090	,0.5878	,0.3090	,0.0000	,-0.3090   
};

float32_t test_input[64] = {0};
float32_t test_output[32] = {0};

#endif
