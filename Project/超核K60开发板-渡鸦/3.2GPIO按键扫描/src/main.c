#include "gpio.h"
#include "common.h"


#define KEY1  PEin(26)
#define LED1  PEout(6)

/* 按键返回状态 0 为按下 1按下 */
#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)
/* 按键返回值 */
static uint8_t gRetValue;

/* 状态机表 */
typedef enum
{
    kKEY_Idle,          /* 空闲态 */
    kKEY_Debounce,      /* 确认与消抖态 */
    kKEY_Confirm,       /* 确认态 */
}KEY_Status;

/* 按键扫描程序 */
static void KEY_Scan(void)
{
    static KEY_Status status = kKEY_Idle;
    switch(status)
    {
        case kKEY_Idle:
            gRetValue = NO_KEY;
            if(KEY1 == 0) /* 如果按键被按下 进入确认与消抖态 */
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce: /* 确认与消抖态 */
            if(KEY1 == 0)
            {
                status = kKEY_Confirm;
            }
            else
            {
                status = kKEY_Idle;
                gRetValue = NO_KEY;
            }
            break;
        case kKEY_Confirm: /* 确认态 */
            if(KEY1 == 1) /* 按键松开 */
            {
                gRetValue = KEY_SINGLE;
                status = kKEY_Idle;
            }
            break;
        default:
            break;
    }
}

int main(void)
{
    /* 初始化 Delay */
    DelayInit();
    /* 将GPIO设置为输入模式 芯片内部自动配置上拉电阻 */
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    /* 设置为输出 */
    GPIO_QuickInit(HW_GPIOE,  6, kGPIO_Mode_OPP);
    while(1)
    {
        KEY_Scan();
        DelayMs(10);
        if(gRetValue == KEY_SINGLE)
        {
            LED1 = !LED1;
        }
    }
}


