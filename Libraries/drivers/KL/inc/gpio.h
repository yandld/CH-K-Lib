#ifndef __CH_LIB_KL_GPIO_H__
#define __CH_LIB_KL_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 

#include <stdint.h>
#include <stdbool.h>

/* GPIO instance define */
#define HW_GPIOA        (0x00U)
#define HW_GPIOB        (0x01U)
#define HW_GPIOC        (0x02U)
#define HW_GPIOD        (0x03U)
#define HW_GPIOE        (0x04U)
#define HW_GPIOF        (0x05U)


#define PIN0      (0x80000000+(1<<0))
#define PIN1      (0x80000000+(1<<1))
#define PIN2      (0x80000000+(1<<2))
#define PIN3      (0x80000000+(1<<3))  
     
typedef enum
{
    kGPIO_Mode_IFT = 0x00,       /* Input floating  */
    kGPIO_Mode_IPD = 0x01,       /* Pull down input */
    kGPIO_Mode_IPU = 0x02,       /* Pull up input    */
    kGPIO_Mode_OPP = 0x04,       /* Push pull output */
}GPIO_Mode_t;


/*!< Interrupts and DMA */
typedef enum
{
    kGPIO_DMA_RisingEdge,	      //上升沿触发DMA
    kGPIO_DMA_FallingEdge,        //下降沿触发DMA
    kGPIO_DMA_RisingFallingEdge,  //上升沿和下降沿触发DMA
    kGPIO_IT_Low,                 //低电平出发中断
    kGPIO_IT_RisingEdge,          //上升沿触发中断
    kGPIO_IT_FallingEdge,         //下降沿触发中断
    kGPIO_IT_RisingFallingEdge,   //上升沿和下降沿触发中断
    kGPIO_IT_High,                //高电平触发中断
}GPIO_ITDMAConfig_Type;


/* 端口中断回调函数定义 */
typedef void (*GPIO_CallBackType)(uint32_t pinxArray);


//!< API functions
uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_Mode_t mode);
void GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir);
void GPIO_Send(uint32_t instance, uint32_t data);
uint32_t GPIO_Get(uint32_t instance);


#ifdef __cplusplus
}
#endif

#endif


