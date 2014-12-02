#include "dsp_complex_demo.h"

 /**
 * @brief  演示arm_cmplx_conj_f32功能
 * @code
 * @endcode
 * @param  None
 * @retval None
 * @author Rein
 * @update 2014-12-2
 */
 
void arm_cmplx_conj_f32_demo (void) {
	float32_t complex_1 [2*2],		/**创建一个浮点数组为了保存复数的实部和虚部*/
			  real_1 = 12.5,		/**为了便于理解，分别创建两个复数的实部和虚部*/
			  image_1 = 3.3,
			  real_2 = 5,
			  image_2 = 2.2,
			  complex_2 [2*2];		/**创建一个数组保存其共轭复数的实部和虚部*/
	complex_1[0] = real_1;			/**给需要计算的复数数组赋值*/
	complex_1[1] = image_1;
	complex_1[2] = real_2;
	complex_1[3] = image_2;
	arm_cmplx_conj_f32(complex_1,complex_2,2);/**求共轭复数*/
	/**打印需要被计算的复数数组*/
	printf("real_1 = %f   image_1 = %fj\n\r",real_1,image_1);
	printf("conj_real_1 = %f   conj_image_1 = %fj\n\r",complex_2[0],complex_2[1]);
	/**打印计算完成的复数数组*/
	printf("real_2 = %f   image_2 = %fj\n\r",real_2,image_2);
	printf("conj_real_2 = %f   conj_image_2 = %fj\n\r",complex_2[2],complex_2[3]);
}
