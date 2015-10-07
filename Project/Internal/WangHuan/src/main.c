#include "gpio.h"
#include "common.h"
#include "uart.h"

#define KEY1  PEin(26)
#define LED1  PEout(6)
#define LED2  PEout(7)
#define LED3  PEout(11)

#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)


#define REV_IDLE        (0x01)
#define REV_LIGHT_ON    (0x03)
#define REV_WAIT        (0x04)

static uint32_t LightOnTime = 500;
static uint32_t WaitTime = 500;
static uint8_t gRetValue;




static void UART_RX_ISR(uint16_t ch)
{
    static uint32_t State = REV_IDLE;
    switch(State)
    {
        case REV_IDLE:
            if(ch == 0x55)
            {
                State = REV_LIGHT_ON;
            }
        break;
        case REV_LIGHT_ON:
            LightOnTime = ch;
            State = REV_WAIT;
        break;
        case REV_WAIT:
            WaitTime = ch;
            State = REV_IDLE;
            printf("Param Configuration: WaitTime:%d  Light on Time:%d\r\n", WaitTime, LightOnTime);
        break;
    }

}



typedef enum
{
    kKEY_Idle,
    kKEY_Debounce,
    kKEY_Confirm,
}KEY_Status;


static void KEY_Scan(void)
{
    static KEY_Status status = kKEY_Idle;
    switch(status)
    {
        case kKEY_Idle:
            gRetValue = NO_KEY;
            if(KEY1 == 0)
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce:
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
        case kKEY_Confirm: 
            if(KEY1 == 1)
            {
                gRetValue = KEY_SINGLE;
                status = kKEY_Idle;
            }
            break;
        default:
            break;
    }
}

void AppFun(void)
{
    LED1 = 1;
    DelayMs(LightOnTime);
    LED1 = 0;
    DelayMs(WaitTime);
    
    LED2 = 1;
    DelayMs(LightOnTime);
    LED2 = 0;
    DelayMs(WaitTime);
    
    LED3 = 1;
    DelayMs(LightOnTime);
    LED3 = 0;
    DelayMs(WaitTime); 
}
 
int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07 , 115200);

    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
    while(1)
    {
        KEY_Scan();
        DelayMs(10);
        if(gRetValue == KEY_SINGLE)
        {
            printf("KEY1 Pressed!\r\n");
            AppFun();
        }
    }
}


