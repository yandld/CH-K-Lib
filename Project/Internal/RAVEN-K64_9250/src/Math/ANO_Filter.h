#ifndef __ANO_FILTER_H
#define __ANO_FILTER_H

#include "ANO_Math.h"

class ANO_Filter
{
	
public:

	ANO_Filter(){
	}

	//一阶低通滤波器系数计算
	float LPF_1st_Factor_Cal(float deltaT, float Fcut);
	
	//互补滤波器系数计算
	float CF_Factor_Cal(float deltaT, float tau);
	
	//一阶低通滤波器
	Vector3f LPF_1st(Vector3f oldData, Vector3f newData, float lpf_factor);

	//一阶互补滤波器
	Vector3f CF_1st(Vector3f gyroData, Vector3f accData, float cf_factor);
	
};


#endif

