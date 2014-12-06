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

 /**
 * @brief  演示arm_cmplx_dot_prod_f32功能
 * @code
 * @endcode
 * @param  None
 * @retval None
 * @author Rein
 * @update 2014-12-2
 */

void arm_cmplx_dot_prod_f32_demo (void) {
	float32_t A[2] = {12.2f,3.5f},
			  B[2] = {4.0f,17.2f},
			  result_real = 0.0f,
			  result_img = 0.0f;
	uint32_t number_of_complex = 1; /**指的是每个数组里面复数的个数，这里分别都是1*/
	
	printf("A=%f +%fj\n\r",A[0],A[1]);
	printf("B=%f +%fj\n\r",B[0],B[1]);
	
	arm_cmplx_dot_prod_f32(A,B,number_of_complex,&result_real,&result_img);

	printf("Result =%f +%fj\n\r",result_real,result_img);

}

 /**
 * @brief  演示arm_cmplx_mag_f32功能
 * @code
 * @endcode
 * @param  None
 * @retval None
 * @author Rein
 * @update 2014-12-2
 */

void arm_cmplx_mag_f32_demo (void) {
	float32_t Complex[6] = {
						10.5f,10.6f,		/**前面是实部，后面是虚部，下同*/
						11.2f,56.3f,
						25.5f,7.2f
					 };
	float32_t result[3] = {0,0,0};			/**保存运算结果*/
	
	arm_cmplx_mag_f32(Complex,result,3);	/**运算*/
	
	printf("Complex_1 = %f + %fj\n\r",Complex[0],Complex[1]);
	printf("Complex_2 = %f + %fj\n\r",Complex[2],Complex[3]);
	printf("Complex_3 = %f + %fj\n\r",Complex[4],Complex[5]);	
	printf("Real of Complex_1 is %f\n\r",result[0]);
	printf("Real of Complex_2 is %f\n\r",result[1]);
	printf("Real of Complex_3 is %f\n\r",result[2]);
}
 
