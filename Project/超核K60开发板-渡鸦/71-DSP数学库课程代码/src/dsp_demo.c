#include "chlib_k.h"
#include "arm_math.h"

 /**
 * @brief  演示arm_abs_f32功能
 * @code
 * @endcode
 * @param  None
 * @retval None
 */
void arm_abs_f32_demo(void) {
	/**	
	*	首先创建一个浮点数向量A(0.0,6.0)
	*	这里可以看做是一个一行两列的行列式，用来表示一个二维向量
	*	当然也可以写成src_vector[1][2] = {{0.0},{6.0}}; 不过请注意在使用过程中注意数组指针的维数。
	*/
	float32_t src_vector[2] = {0.0,6.0};	
	/**再创建一个浮点数数组用来存放运算后的结果,习惯性将局部变量申请后初始化为0*/
	float32_t dst_vector[2] = {0.0,0.0};
	/**串口打印初始化结果*/
	printf("src_vector[0] = %f\r src_vector[1] = %f\n\r");
	printf("dst_vector[0] = %f\r dst_vector[1] = %f\n\r");
	/**调用数学库*/
	printf("start arm_abs...\n\r");
	/**sizeof(src_vector)/sizeof(float32_t) 表示先求出数组所占用字节数然后除以每个元素占用字节数 结果是数组元素长度*/
	arm_abs_f32(src_vector,dst_vector,sizeof(src_vector)/sizeof(float32_t));
	printf("arm_abs completed\n\r");
	/**打印输出结果进行比较*/
	printf("dst_vector[0] = %f\r dst_vector[1] = %f\n\r");	
}

